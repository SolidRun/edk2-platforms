/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright 2020 NXP
*  Copyright 2020 Puresoftware Ltd
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

Scope(\_SB.FMN0)
{
  Device(MAC3) {
    Name(_HID, "NXP0025")
    Name(_UID, 5)
    Name(_CCA, 1) // Cache Coherency Attribute
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x1aea000, 0x1000)
    }) // end of _CRS for MAC5
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"mac-address", Package (6) {11,22,33,44,55,66}},
        Package () {"cell-index", 0x5},
        Package () {"phy-handle", \_SB.FMN0.MDI0.PHY4},
        Package () {"phy-connection-type", "sgmii"},
        Package () {"compatible", "fman-memac"},
        Package () {"fsl,fman-ports", Package () {\_SB.FMN0.PRX5,\_SB.FMN0.PTX5}},
        Package () {"pcsphy-handle", \_SB.FMN0.MDI5.PCS5}
      }
    })
  }

  Device(MDI5) {
    Name(_HID, "NXP0006")
    Name(_UID, 5)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x1aeb000, 0x1000)
    }) // end of _CRS for MDI5

    Device(PCS5) {
      Name (_ADR, 0x0)
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 0x0}
        }
      })
    } // end of PCS5
  } // end of MDI5

  Device(PRX5) {
    Name(_HID, "NXP0026")
    Name(_UID, 6)
    Name(_CCA, 1) // Cache Coherency Attribute
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x1a8d000, 0x1000)
    }) // end of _CRS for PRX5
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"cell-index", 0xd},
        Package () {"compatible", "fman-v3-port-rx"}
      }
    })
  } // PRX5 end

  Device(PTX5) {
    Name(_HID, "NXP0026")
    Name(_UID, 7)
    Name(_CCA, 1) // Cache Coherency Attribute
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x1aad000, 0x1000)
    }) // end of _CRS for PTX5
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"cell-index", 0x2d},
        Package () {"compatible", "fman-v3-port-tx"},
        Package () {"fsl,qman-channel-id", 0x807}
      }
    })
  } // PTX5 end
}
