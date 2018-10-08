/** NxpQoriqLsMem.c
*
*  Board memory specific Library.
*
*  Based on BeagleBoardPkg/Library/BeagleBoardLib/BeagleBoardMem.c
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.
*  Copyright 2018 NXP
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>

#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS          25

//
// Calculate the MC (Management Complex) base address and DDR size based on
// if the MC is loaded in DDR low memory region or in DDR high memory region.
//
#if FixedPcdGetBool (PcdMcHighMemSupport)
#define DDR_MEM_SIZE                            FixedPcdGet64 (PcdDramMemSize) - FixedPcdGet64 (PcdDpaa2McRamSize)
#define MC_BASE_ADDR                            FixedPcdGet64 (PcdDram2BaseAddr) + DDR_MEM_SIZE
#else
#define DDR_MEM_SIZE                            FixedPcdGet64 (PcdDramMemSize)
#define MC_BASE_ADDR                            PcdGet64 (PcdSystemMemoryBase) - FixedPcdGet64 (PcdDpaa2McRamSize)
#endif


/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param  VirtualMemoryMap     Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                               Virtual Memory mapping. This array must be ended by a zero-filled
                               entry

**/

VOID
ArmPlatformGetVirtualMemoryMap (
  IN  ARM_MEMORY_REGION_DESCRIPTOR ** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_ATTRIBUTES     CacheAttributes;
  UINTN                            Index;
  ARM_MEMORY_REGION_DESCRIPTOR     *VirtualMemoryTable;
  UINTN I;
  Index = 0;
  I = 0;

  ASSERT (VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages (
          EFI_SIZE_TO_PAGES (sizeof (ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));

  if (VirtualMemoryTable == NULL) {
    return;
  }

  CacheAttributes = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;

  // DRAM1 (Must be 1st entry)
  VirtualMemoryTable[Index].PhysicalBase = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase  = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length       = PcdGet64 (PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // CCSR Space
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdCcsrBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdCcsrBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdCcsrSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // FSPI region 1
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdQspiRegionBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdQspiRegionBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdQspiRegionSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // FSPI region 2
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdQspiRegion2BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdQspiRegion2BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdQspiRegion2Size);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // DRAM2
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdDram2BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdDram2BaseAddr);
  VirtualMemoryTable[Index].Length       = DDR_MEM_SIZE;
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // MC private DRAM
  VirtualMemoryTable[++Index].PhysicalBase = MC_BASE_ADDR;
  VirtualMemoryTable[Index].VirtualBase  = MC_BASE_ADDR;
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdDpaa2McRamSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Map for PCIe. 6 PCI at equal space
  for (I = 0; I < 6; I++) {
    // PCIe1
    VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdPciExp1BaseAddr) + I * FixedPcdGet64 (PcdPciExp1BaseSize);
    VirtualMemoryTable[Index].VirtualBase  = VirtualMemoryTable[Index].PhysicalBase;
    VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdPciExp1BaseSize);
    VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  }

  // DPAA2 MC Portals region
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdDpaa2McPortalBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdDpaa2McPortalBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdDpaa2McPortalSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DPAA2 NI Portals region
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdDpaa2NiPortalsBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdDpaa2NiPortalsBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdDpaa2NiPortalsSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DPAA2 QBMAN Portals - cache enabled region
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdDpaa2QBmanPortalsBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdDpaa2QBmanPortalsBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdDpaa2QBmanPortalsCacheSize);
  VirtualMemoryTable[Index].Attributes   = CacheAttributes;

  // DPAA2 QBMAN Portals - cache inhibited region
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdDpaa2QBmanPortalsBaseAddr) + FixedPcdGet64 (PcdDpaa2QBmanPortalsCacheSize);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdDpaa2QBmanPortalsBaseAddr) + FixedPcdGet64 (PcdDpaa2QBmanPortalsCacheSize);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdDpaa2QBmanPortalSize) - FixedPcdGet64 (PcdDpaa2QBmanPortalsCacheSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT ((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}