/** @file
  Include file for PCI Host Bridge Library instance for NXP SoCs.

  Copyright 2018 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PCIE_HOST_BRIDGE_LIB_H_
#define __PCIE_HOST_BRIDGE_LIB_H_

#ifndef BIT
#define BIT(nr)         (1 << (nr))
#endif

/* PAB Interrupt registers */
#define PAB_INTP_AMBA_MISC_ENB          0x0b0c
#define PAB_INTP_AMBA_MISC_STAT         0x0b1c
#define  PAB_INTP_RESET                 BIT(1)
#define  PAB_INTP_MSI                   BIT(3)
#define  PAB_INTP_INTA                  BIT(5)
#define  PAB_INTP_INTB                  BIT(6)
#define  PAB_INTP_INTC                  BIT(7)
#define  PAB_INTP_INTD                  BIT(8)
#define  PAB_INTP_PCIE_UE               BIT(9)
#define  PAB_INTP_IE_PMREDI             BIT(29)
#define  PAB_INTP_IE_EC                 BIT(30)
#define  PAB_INTP_MSI_MASK              PAB_INTP_MSI
#define  PAB_INTP_INTX_MASK             (PAB_INTP_INTA | PAB_INTP_INTB | \
                                         PAB_INTP_INTC | PAB_INTP_INTD)

/* LUT registers */
#define PCIE_LUT_ENABLE		        BIT(31)

typedef struct __LS_PCIE_LUT {
  UINT8        Reserved0[0x20];
  UINT32       PexLsr;   // PEX LUT Status Register
  UINT32       PexLcr;   // PEX LUT Control Register
  UINT8        Reserved28[0x800 - 0x28];
  struct {
    UINT32     PexLudr; // PEX LUT Entry Upper Data Register
    UINT32     PexLldr; // PEX LUT Entry Lower Data Register
  } PexLut[32];
} LS_PCIE_LUT;

typedef struct __LS_PCIE {
  ///
  /// Base Address of Controller registers
  ///
  UINTN          ControllerAddress;
  ///
  /// Next Available Look Up Table entry Index
  ///
  INT32          NextLutIndex;
  /// Next Available IATU Window
  ///
  INT8            NextIatuIndex;
  ///
  /// The stream id index corresponding to BusDevFunc for
  /// this PCIE controller.
  ///
  INT32           CurrentStreamId;
  ///
  /// PCIE controller index in system (This number is 0 based)
  ///
  INT32           ControllerIndex;
  ///
  /// Look Up Table
  ///
  LS_PCIE_LUT    *LsPcieLut;
} LS_PCIE;

#endif // __PCIE_HOST_BRIDGE_LIB_H_
