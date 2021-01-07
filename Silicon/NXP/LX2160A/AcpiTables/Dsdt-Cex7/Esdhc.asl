/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
  Copyright 2017-2020 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/

Scope(_SB)
{
  Device(SDC0) {
    Name(_HID, "NXP0003")
    Name(_CCA, 1)
    Name(_UID, 0)
    Name(CLK, 0)
    Name(_DEP, Package() {\_SB.PCLK})
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, SDC0_BASE, SDC_LEN)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive)
       {
         SDC0_IT
       }
    })
    Method(_INI, 0, NotSerialized) {
      Store(\_SB.PCLK.CLK, CLK)
    }
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
         Package () {"max-frequency", 100000000},
         Package () {"clock-frequency", CLK},
         Package () {"little-endian", 1},
         Package () {"sdhci,auto-cmd12", 1},
         Package () {"bus-width", 4},
         Package () {"sd-uhs-sdr50", 1},
      }
    })
  }

  Device(SDC1) {
    Name(_HID, "NXP0003")
    Name(_CCA, 1)
    Name(_UID, 1)
    Name(CLK, 0)
    Name(_DEP, Package() {\_SB.PCLK})
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, SDC1_BASE, SDC_LEN)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive)
       {
         SDC1_IT
       }
    })
    Method(_INI, 0, NotSerialized) {
      Store(\_SB.PCLK.CLK, CLK)
    }
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
         Package () {"max-frequency", 200000000},
         Package () {"clock-frequency", CLK},
         Package () {"little-endian", 1},
         Package () {"sdhci,auto-cmd12", 1},
         Package () {"non-removable", 1},
         Package () {"bus-width", 8},
         Package () {"mmc-hs200-1_8v", 1},
      }
    })
  }
}
