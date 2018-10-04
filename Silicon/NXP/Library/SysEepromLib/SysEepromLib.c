/** @file  SysEepromLib.c

  SystemID Non-Volatile Memory Device

  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/I2c.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SysEepromLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "SysEeprom.h"

SYSTEM_ID  *SystemID;

/**
  Read EEPROM data

  This function reads EEPROM device data connected to I2c Bus.
  If data is already read once the it is not read again unless ForceRead flag is not set.
  The data is checked for CRC integrity if ForceRead flag is set.
  The supported Eeprom data types are CCID and NXID (v0 or v1).
  This function must be called after relocation to Ram.

  @param[in]  ForceRead - Read the data from Eeprom device connected to I2c Bus,
                          even if the data is alrady read.

  @retval EFI_OUT_OF_RESOURCES   Not able to allocate memory for SYSTEM_ID
                                 structure.
  @retval EFI_CRC_ERROR          CRC checksum not mached
  @retval EFI_NOT_FOUND          No Meaningful data found on eeprom
  @retval EFI_SUCCESS            Retrieved eeprom data successfully
**/
EFI_STATUS
EepromRead (
  IN  BOOLEAN   ForceRead
  )
{
  EFI_STATUS   Status;
  UINT32       CalulatedCrc32;
  UINT32       CrcOffset;
  UINT32       CrcStored;

  Status = EFI_SUCCESS;
  CalulatedCrc32 = 0;
  CrcOffset = 0;
  CrcStored = 0;

  if (SystemID == NULL) {
    SystemID = (SYSTEM_ID *)AllocateZeroPool (sizeof (SYSTEM_ID));
    if (SystemID == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    ForceRead = TRUE;
  }

  if (ForceRead == TRUE) {
    Status = I2cBusInit(PcdGet32(PcdSysEepromI2cBus), PcdGet32(PcdI2cSpeed));
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = I2cDataRead(
               PcdGet32(PcdSysEepromI2cBus),
               PcdGet32(PcdSysEepromI2cAddress),
               0,
               0x1,
               (UINT8 *)SystemID,
               sizeof(SYSTEM_ID)
             );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  if (IS_VALID_NXID(SystemID->NXSystemID.TagID)) {
    if (SystemID->NXSystemID.Version == 0) {
      // MAC address #9 in v1 occupies the same position as the CRC in v0
      CrcOffset = OFFSET_OF(NX_SYSTEM_ID, Mac[8][0]);
      CrcStored = SwapBytes32 (*(UINT32 *)&SystemID->NXSystemID.Mac[8][0]);
    } else {
      CrcOffset = OFFSET_OF(NX_SYSTEM_ID, Crc32);
      CrcStored = SwapBytes32 (*(UINT32 *)&SystemID->NXSystemID.Crc32);
    }
  } else if (IS_VALID_CCID(SystemID->CCSystemID.TagID)) {
    CrcOffset = OFFSET_OF(CC_SYSTEM_ID, Crc32);
    CrcStored = SwapBytes32 (*(UINT32 *)&SystemID->CCSystemID.Crc32);
  } else {
    return EFI_NOT_FOUND;
  }

  if (ForceRead == TRUE) {
    Status = gBS->CalculateCrc32 (
                    SystemID,
                    CrcOffset,
                    &CalulatedCrc32
                  );
    if (EFI_ERROR(Status)) {
      ZeroMem(SystemID, sizeof (SYSTEM_ID));
      return Status;
    }
    if (CalulatedCrc32 != CrcStored) {
      ZeroMem(SystemID, sizeof (SYSTEM_ID));
      return EFI_CRC_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  Read the MAC addresses from EEPROM

  This function reads the MAC addresses from EEPROM.
  This function must be called after relocation to ram.

  @param[in]  MacNo        Mac number whose address to be read from Eeprom
                           This is zero based. i.e. 0, 1, 2 etc
  @param[out] MacAddress   Buffer of atleast 6 bytes to hold Mac address
                           (Must not be NULL)

  @retval EFI_INVALID_PARAMETER  Invalid MacNo or Null MacAddress
  @retval EFI_OUT_OF_RESOURCES   Not able to allocate memory for SYSTEM_ID
                                 structure.
  @retval EFI_CRC_ERROR          CRC checksum not mached
  @retval EFI_NOT_FOUND          MacAddress for MacNo not found
  @retval EFI_SUCCESS            Retrieved MacAddress successfully
**/
EFI_STATUS
EFIAPI
MacReadFromEeprom (
  IN   UINT8   MacNo,
  OUT  VOID    *MacAddress
  )
{
  EFI_STATUS Status;

  if (MacAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EepromRead (EEPROM_NO_FORCE_READ);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (IS_VALID_NXID(SystemID->NXSystemID.TagID)) {
    if (MacNo >= SystemID->NXSystemID.MacSize) {
      return EFI_NOT_FOUND;
    }
    CopyMem (MacAddress, SystemID->NXSystemID.Mac[MacNo], 6);
  } else {
    if (MacNo > SystemID->CCSystemID.MacSize) {
      return EFI_NOT_FOUND;
    }
    CopyMem (MacAddress, SystemID->CCSystemID.Mac[MacNo], 6);
  }

  return EFI_SUCCESS;
}

