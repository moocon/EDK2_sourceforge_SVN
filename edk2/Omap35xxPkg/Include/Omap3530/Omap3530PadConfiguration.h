/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OMAP3530_PAD_CONFIGURATION_H__
#define __OMAP3530_PAD_CONFIGURATION_H__

#define SYSTEM_CONTROL_MODULE_BASE  0x48002000

//Pin definition
#define SDRC_D0             (SYSTEM_CONTROL_MODULE_BASE + 0x030)
#define SDRC_D1             (SYSTEM_CONTROL_MODULE_BASE + 0x032)
#define SDRC_D2             (SYSTEM_CONTROL_MODULE_BASE + 0x034)
#define SDRC_D3             (SYSTEM_CONTROL_MODULE_BASE + 0x036)
#define SDRC_D4             (SYSTEM_CONTROL_MODULE_BASE + 0x038)
#define SDRC_D5             (SYSTEM_CONTROL_MODULE_BASE + 0x03A)
#define SDRC_D6             (SYSTEM_CONTROL_MODULE_BASE + 0x03C)
#define SDRC_D7             (SYSTEM_CONTROL_MODULE_BASE + 0x03E)
#define SDRC_D8             (SYSTEM_CONTROL_MODULE_BASE + 0x040)
#define SDRC_D9             (SYSTEM_CONTROL_MODULE_BASE + 0x042)
#define SDRC_D10            (SYSTEM_CONTROL_MODULE_BASE + 0x044)
#define SDRC_D11            (SYSTEM_CONTROL_MODULE_BASE + 0x046)
#define SDRC_D12            (SYSTEM_CONTROL_MODULE_BASE + 0x048)
#define SDRC_D13            (SYSTEM_CONTROL_MODULE_BASE + 0x04A)
#define SDRC_D14            (SYSTEM_CONTROL_MODULE_BASE + 0x04C)
#define SDRC_D15            (SYSTEM_CONTROL_MODULE_BASE + 0x04E)
#define SDRC_D16            (SYSTEM_CONTROL_MODULE_BASE + 0x050)
#define SDRC_D17            (SYSTEM_CONTROL_MODULE_BASE + 0x052)
#define SDRC_D18            (SYSTEM_CONTROL_MODULE_BASE + 0x054)
#define SDRC_D19            (SYSTEM_CONTROL_MODULE_BASE + 0x056)
#define SDRC_D20            (SYSTEM_CONTROL_MODULE_BASE + 0x058)
#define SDRC_D21            (SYSTEM_CONTROL_MODULE_BASE + 0x05A)
#define SDRC_D22            (SYSTEM_CONTROL_MODULE_BASE + 0x05C)
#define SDRC_D23            (SYSTEM_CONTROL_MODULE_BASE + 0x05E)
#define SDRC_D24            (SYSTEM_CONTROL_MODULE_BASE + 0x060)
#define SDRC_D25            (SYSTEM_CONTROL_MODULE_BASE + 0x062)
#define SDRC_D26            (SYSTEM_CONTROL_MODULE_BASE + 0x064)
#define SDRC_D27            (SYSTEM_CONTROL_MODULE_BASE + 0x066)
#define SDRC_D28            (SYSTEM_CONTROL_MODULE_BASE + 0x068)
#define SDRC_D29            (SYSTEM_CONTROL_MODULE_BASE + 0x06A)
#define SDRC_D30            (SYSTEM_CONTROL_MODULE_BASE + 0x06C)
#define SDRC_D31            (SYSTEM_CONTROL_MODULE_BASE + 0x06E)
#define SDRC_CLK            (SYSTEM_CONTROL_MODULE_BASE + 0x070)
#define SDRC_DQS0           (SYSTEM_CONTROL_MODULE_BASE + 0x072)
#define SDRC_CKE0           (SYSTEM_CONTROL_MODULE_BASE + 0x262)
#define SDRC_CKE1           (SYSTEM_CONTROL_MODULE_BASE + 0x264)
#define SDRC_DQS1           (SYSTEM_CONTROL_MODULE_BASE + 0x074)
#define SDRC_DQS2           (SYSTEM_CONTROL_MODULE_BASE + 0x076)
#define SDRC_DQS3           (SYSTEM_CONTROL_MODULE_BASE + 0x078)
#define GPMC_A1             (SYSTEM_CONTROL_MODULE_BASE + 0x07A)
#define GPMC_A2             (SYSTEM_CONTROL_MODULE_BASE + 0x07C)
#define GPMC_A3             (SYSTEM_CONTROL_MODULE_BASE + 0x07E)
#define GPMC_A4             (SYSTEM_CONTROL_MODULE_BASE + 0x080)
#define GPMC_A5             (SYSTEM_CONTROL_MODULE_BASE + 0x082)
#define GPMC_A6             (SYSTEM_CONTROL_MODULE_BASE + 0x084)
#define GPMC_A7             (SYSTEM_CONTROL_MODULE_BASE + 0x086)
#define GPMC_A8             (SYSTEM_CONTROL_MODULE_BASE + 0x088)
#define GPMC_A9             (SYSTEM_CONTROL_MODULE_BASE + 0x08A)
#define GPMC_A10            (SYSTEM_CONTROL_MODULE_BASE + 0x08C)
#define GPMC_D0             (SYSTEM_CONTROL_MODULE_BASE + 0x08E)
#define GPMC_D1             (SYSTEM_CONTROL_MODULE_BASE + 0x090)
#define GPMC_D2             (SYSTEM_CONTROL_MODULE_BASE + 0x092)
#define GPMC_D3             (SYSTEM_CONTROL_MODULE_BASE + 0x094)
#define GPMC_D4             (SYSTEM_CONTROL_MODULE_BASE + 0x096)
#define GPMC_D5             (SYSTEM_CONTROL_MODULE_BASE + 0x098)
#define GPMC_D6             (SYSTEM_CONTROL_MODULE_BASE + 0x09A)
#define GPMC_D7             (SYSTEM_CONTROL_MODULE_BASE + 0x09C)
#define GPMC_D8             (SYSTEM_CONTROL_MODULE_BASE + 0x09E)
#define GPMC_D9             (SYSTEM_CONTROL_MODULE_BASE + 0x0A0)
#define GPMC_D10            (SYSTEM_CONTROL_MODULE_BASE + 0x0A2)
#define GPMC_D11            (SYSTEM_CONTROL_MODULE_BASE + 0x0A4)
#define GPMC_D12            (SYSTEM_CONTROL_MODULE_BASE + 0x0A6)
#define GPMC_D13            (SYSTEM_CONTROL_MODULE_BASE + 0x0A8)
#define GPMC_D14            (SYSTEM_CONTROL_MODULE_BASE + 0x0AA)
#define GPMC_D15            (SYSTEM_CONTROL_MODULE_BASE + 0x0AC)
#define GPMC_NCS0           (SYSTEM_CONTROL_MODULE_BASE + 0x0AE)
#define GPMC_NCS1           (SYSTEM_CONTROL_MODULE_BASE + 0x0B0)
#define GPMC_NCS2           (SYSTEM_CONTROL_MODULE_BASE + 0x0B2)
#define GPMC_NCS3           (SYSTEM_CONTROL_MODULE_BASE + 0x0B4)
#define GPMC_NCS4           (SYSTEM_CONTROL_MODULE_BASE + 0x0B6)
#define GPMC_NCS5           (SYSTEM_CONTROL_MODULE_BASE + 0x0B8)
#define GPMC_NCS6           (SYSTEM_CONTROL_MODULE_BASE + 0x0BA)
#define GPMC_NCS7           (SYSTEM_CONTROL_MODULE_BASE + 0x0BC)
#define GPMC_CLK            (SYSTEM_CONTROL_MODULE_BASE + 0x0BE)
#define GPMC_NADV_ALE       (SYSTEM_CONTROL_MODULE_BASE + 0x0C0)
#define GPMC_NOE            (SYSTEM_CONTROL_MODULE_BASE + 0x0C2)
#define GPMC_NWE            (SYSTEM_CONTROL_MODULE_BASE + 0x0C4)
#define GPMC_NBE0_CLE       (SYSTEM_CONTROL_MODULE_BASE + 0x0C6)
#define GPMC_NBE1           (SYSTEM_CONTROL_MODULE_BASE + 0x0C8)
#define GPMC_NWP            (SYSTEM_CONTROL_MODULE_BASE + 0x0CA)
#define GPMC_WAIT0          (SYSTEM_CONTROL_MODULE_BASE + 0x0CC)
#define GPMC_WAIT1          (SYSTEM_CONTROL_MODULE_BASE + 0x0CE)
#define GPMC_WAIT2          (SYSTEM_CONTROL_MODULE_BASE + 0x0D0)
#define GPMC_WAIT3          (SYSTEM_CONTROL_MODULE_BASE + 0x0D2)
#define DSS_PCLK            (SYSTEM_CONTROL_MODULE_BASE + 0x0D4)
#define DSS_HSYNC           (SYSTEM_CONTROL_MODULE_BASE + 0x0D6)
#define DSS_PSYNC           (SYSTEM_CONTROL_MODULE_BASE + 0x0D8)
#define DSS_ACBIAS          (SYSTEM_CONTROL_MODULE_BASE + 0x0DA)
#define DSS_DATA0           (SYSTEM_CONTROL_MODULE_BASE + 0x0DC)
#define DSS_DATA1           (SYSTEM_CONTROL_MODULE_BASE + 0x0DE)
#define DSS_DATA2           (SYSTEM_CONTROL_MODULE_BASE + 0x0E0)
#define DSS_DATA3           (SYSTEM_CONTROL_MODULE_BASE + 0x0E2)
#define DSS_DATA4           (SYSTEM_CONTROL_MODULE_BASE + 0x0E4)
#define DSS_DATA5           (SYSTEM_CONTROL_MODULE_BASE + 0x0E6)
#define DSS_DATA6           (SYSTEM_CONTROL_MODULE_BASE + 0x0E8)
#define DSS_DATA7           (SYSTEM_CONTROL_MODULE_BASE + 0x0EA)
#define DSS_DATA8           (SYSTEM_CONTROL_MODULE_BASE + 0x0EC)
#define DSS_DATA9           (SYSTEM_CONTROL_MODULE_BASE + 0x0EE)
#define DSS_DATA10          (SYSTEM_CONTROL_MODULE_BASE + 0x0F0)
#define DSS_DATA11          (SYSTEM_CONTROL_MODULE_BASE + 0x0F2)
#define DSS_DATA12          (SYSTEM_CONTROL_MODULE_BASE + 0x0F4)
#define DSS_DATA13          (SYSTEM_CONTROL_MODULE_BASE + 0x0F6)
#define DSS_DATA14          (SYSTEM_CONTROL_MODULE_BASE + 0x0F8)
#define DSS_DATA15          (SYSTEM_CONTROL_MODULE_BASE + 0x0FA)
#define DSS_DATA16          (SYSTEM_CONTROL_MODULE_BASE + 0x0FC)
#define DSS_DATA17          (SYSTEM_CONTROL_MODULE_BASE + 0x0FE)
#define DSS_DATA18          (SYSTEM_CONTROL_MODULE_BASE + 0x100)
#define DSS_DATA19          (SYSTEM_CONTROL_MODULE_BASE + 0x102)
#define DSS_DATA20          (SYSTEM_CONTROL_MODULE_BASE + 0x104)
#define DSS_DATA21          (SYSTEM_CONTROL_MODULE_BASE + 0x106)
#define DSS_DATA22          (SYSTEM_CONTROL_MODULE_BASE + 0x108)
#define DSS_DATA23          (SYSTEM_CONTROL_MODULE_BASE + 0x10A)
#define CAM_HS              (SYSTEM_CONTROL_MODULE_BASE + 0x10C)
#define CAM_VS              (SYSTEM_CONTROL_MODULE_BASE + 0x10E)
#define CAM_XCLKA           (SYSTEM_CONTROL_MODULE_BASE + 0x110)
#define CAM_PCLK            (SYSTEM_CONTROL_MODULE_BASE + 0x112)
#define CAM_FLD             (SYSTEM_CONTROL_MODULE_BASE + 0x114)
#define CAM_D0              (SYSTEM_CONTROL_MODULE_BASE + 0x116)
#define CAM_D1              (SYSTEM_CONTROL_MODULE_BASE + 0x118)
#define CAM_D2              (SYSTEM_CONTROL_MODULE_BASE + 0x11A)
#define CAM_D3              (SYSTEM_CONTROL_MODULE_BASE + 0x11C)
#define CAM_D4              (SYSTEM_CONTROL_MODULE_BASE + 0x11E)
#define CAM_D5              (SYSTEM_CONTROL_MODULE_BASE + 0x120)
#define CAM_D6              (SYSTEM_CONTROL_MODULE_BASE + 0x122)
#define CAM_D7              (SYSTEM_CONTROL_MODULE_BASE + 0x124)
#define CAM_D8              (SYSTEM_CONTROL_MODULE_BASE + 0x126)
#define CAM_D9              (SYSTEM_CONTROL_MODULE_BASE + 0x128)
#define CAM_D10             (SYSTEM_CONTROL_MODULE_BASE + 0x12A)
#define CAM_D11             (SYSTEM_CONTROL_MODULE_BASE + 0x12C)
#define CAM_XCLKB           (SYSTEM_CONTROL_MODULE_BASE + 0x12E)
#define CAM_WEN             (SYSTEM_CONTROL_MODULE_BASE + 0x130)
#define CAM_STROBE          (SYSTEM_CONTROL_MODULE_BASE + 0x132)
#define CSI2_DX0            (SYSTEM_CONTROL_MODULE_BASE + 0x134)
#define CSI2_DY0            (SYSTEM_CONTROL_MODULE_BASE + 0x136)
#define CSI2_DX1            (SYSTEM_CONTROL_MODULE_BASE + 0x138)
#define CSI2_DY1            (SYSTEM_CONTROL_MODULE_BASE + 0x13A)
#define MCBSP2_FSX          (SYSTEM_CONTROL_MODULE_BASE + 0x13C)
#define MCBSP2_CLKX         (SYSTEM_CONTROL_MODULE_BASE + 0x13E)
#define MCBSP2_DR           (SYSTEM_CONTROL_MODULE_BASE + 0x140)
#define MCBSP2_DX           (SYSTEM_CONTROL_MODULE_BASE + 0x142)
#define MMC1_CLK            (SYSTEM_CONTROL_MODULE_BASE + 0x144)
#define MMC1_CMD            (SYSTEM_CONTROL_MODULE_BASE + 0x146)
#define MMC1_DAT0           (SYSTEM_CONTROL_MODULE_BASE + 0x148)
#define MMC1_DAT1           (SYSTEM_CONTROL_MODULE_BASE + 0x14A)
#define MMC1_DAT2           (SYSTEM_CONTROL_MODULE_BASE + 0x14C)
#define MMC1_DAT3           (SYSTEM_CONTROL_MODULE_BASE + 0x14E)
#define MMC1_DAT4           (SYSTEM_CONTROL_MODULE_BASE + 0x150)
#define MMC1_DAT5           (SYSTEM_CONTROL_MODULE_BASE + 0x152)
#define MMC1_DAT6           (SYSTEM_CONTROL_MODULE_BASE + 0x154)
#define MMC1_DAT7           (SYSTEM_CONTROL_MODULE_BASE + 0x156)
#define MMC2_CLK            (SYSTEM_CONTROL_MODULE_BASE + 0x158)
#define MMC2_CMD            (SYSTEM_CONTROL_MODULE_BASE + 0x15A)
#define MMC2_DAT0           (SYSTEM_CONTROL_MODULE_BASE + 0x15C)
#define MMC2_DAT1           (SYSTEM_CONTROL_MODULE_BASE + 0x15E)
#define MMC2_DAT2           (SYSTEM_CONTROL_MODULE_BASE + 0x160)
#define MMC2_DAT3           (SYSTEM_CONTROL_MODULE_BASE + 0x162)
#define MMC2_DAT4           (SYSTEM_CONTROL_MODULE_BASE + 0x164)
#define MMC2_DAT5           (SYSTEM_CONTROL_MODULE_BASE + 0x166)
#define MMC2_DAT6           (SYSTEM_CONTROL_MODULE_BASE + 0x168)
#define MMC2_DAT7           (SYSTEM_CONTROL_MODULE_BASE + 0x16A)
#define MCBSP3_DX           (SYSTEM_CONTROL_MODULE_BASE + 0x16C)
#define MCBSP3_DR           (SYSTEM_CONTROL_MODULE_BASE + 0x16E)
#define MCBSP3_CLKX         (SYSTEM_CONTROL_MODULE_BASE + 0x170)
#define MCBSP3_FSX          (SYSTEM_CONTROL_MODULE_BASE + 0x172)
#define UART2_CTS           (SYSTEM_CONTROL_MODULE_BASE + 0x174)
#define UART2_RTS           (SYSTEM_CONTROL_MODULE_BASE + 0x176)
#define UART2_TX            (SYSTEM_CONTROL_MODULE_BASE + 0x178)
#define UART2_RX            (SYSTEM_CONTROL_MODULE_BASE + 0x17A)
#define UART1_TX            (SYSTEM_CONTROL_MODULE_BASE + 0x17C)
#define UART1_RTS           (SYSTEM_CONTROL_MODULE_BASE + 0x17E)
#define UART1_CTS           (SYSTEM_CONTROL_MODULE_BASE + 0x180)
#define UART1_RX            (SYSTEM_CONTROL_MODULE_BASE + 0x182)
#define MCBSP4_CLKX         (SYSTEM_CONTROL_MODULE_BASE + 0x184)
#define MCBSP4_DR           (SYSTEM_CONTROL_MODULE_BASE + 0x186)
#define MCBSP4_DX           (SYSTEM_CONTROL_MODULE_BASE + 0x188)
#define MCBSP4_FSX          (SYSTEM_CONTROL_MODULE_BASE + 0x18A)
#define MCBSP1_CLKR         (SYSTEM_CONTROL_MODULE_BASE + 0x18C)
#define MCBSP1_FSR          (SYSTEM_CONTROL_MODULE_BASE + 0x18E)
#define MCBSP1_DX           (SYSTEM_CONTROL_MODULE_BASE + 0x190)
#define MCBSP1_DR           (SYSTEM_CONTROL_MODULE_BASE + 0x192)
#define MCBSP1_CLKS         (SYSTEM_CONTROL_MODULE_BASE + 0x194)
#define MCBSP1_FSX          (SYSTEM_CONTROL_MODULE_BASE + 0x196)
#define MCBSP1_CLKX         (SYSTEM_CONTROL_MODULE_BASE + 0x198)
#define UART3_CTS_RCTX      (SYSTEM_CONTROL_MODULE_BASE + 0x19A)
#define UART3_RTS_SD        (SYSTEM_CONTROL_MODULE_BASE + 0x19C)
#define UART3_RX_IRRX       (SYSTEM_CONTROL_MODULE_BASE + 0x19E)
#define UART3_TX_IRTX       (SYSTEM_CONTROL_MODULE_BASE + 0x1A0)
#define HSUSB0_CLK          (SYSTEM_CONTROL_MODULE_BASE + 0x1A2)
#define HSUSB0_STP          (SYSTEM_CONTROL_MODULE_BASE + 0x1A4)
#define HSUSB0_DIR          (SYSTEM_CONTROL_MODULE_BASE + 0x1A6)
#define HSUSB0_NXT          (SYSTEM_CONTROL_MODULE_BASE + 0x1A8)
#define HSUSB0_DATA0        (SYSTEM_CONTROL_MODULE_BASE + 0x1AA)
#define HSUSB0_DATA1        (SYSTEM_CONTROL_MODULE_BASE + 0x1AC)
#define HSUSB0_DATA2        (SYSTEM_CONTROL_MODULE_BASE + 0x1AE)
#define HSUSB0_DATA3        (SYSTEM_CONTROL_MODULE_BASE + 0x1B0)
#define HSUSB0_DATA4        (SYSTEM_CONTROL_MODULE_BASE + 0x1B2)
#define HSUSB0_DATA5        (SYSTEM_CONTROL_MODULE_BASE + 0x1B4)
#define HSUSB0_DATA6        (SYSTEM_CONTROL_MODULE_BASE + 0x1B6)
#define HSUSB0_DATA7        (SYSTEM_CONTROL_MODULE_BASE + 0x1B8)
#define I2C1_SCL            (SYSTEM_CONTROL_MODULE_BASE + 0x1BA)
#define I2C1_SDA            (SYSTEM_CONTROL_MODULE_BASE + 0x1BC)
#define I2C2_SCL            (SYSTEM_CONTROL_MODULE_BASE + 0x1BE)
#define I2C2_SDA            (SYSTEM_CONTROL_MODULE_BASE + 0x1C0)
#define I2C3_SCL            (SYSTEM_CONTROL_MODULE_BASE + 0x1C2)
#define I2C3_SDA            (SYSTEM_CONTROL_MODULE_BASE + 0x1C4)
#define HDQ_SIO             (SYSTEM_CONTROL_MODULE_BASE + 0x1C6)
#define MCSPI1_CLK          (SYSTEM_CONTROL_MODULE_BASE + 0x1C8)
#define MCSPI1_SIMO         (SYSTEM_CONTROL_MODULE_BASE + 0x1CA)
#define MCSPI1_SOMI         (SYSTEM_CONTROL_MODULE_BASE + 0x1CC)
#define MCSPI1_CS0          (SYSTEM_CONTROL_MODULE_BASE + 0x1CE)
#define MCSPI1_CS1          (SYSTEM_CONTROL_MODULE_BASE + 0x1D0)
#define MCSPI1_CS2          (SYSTEM_CONTROL_MODULE_BASE + 0x1D2)
#define MCSPI1_CS3          (SYSTEM_CONTROL_MODULE_BASE + 0x1D4)
#define MCSPI2_CLK          (SYSTEM_CONTROL_MODULE_BASE + 0x1D6)
#define MCSPI2_SIMO         (SYSTEM_CONTROL_MODULE_BASE + 0x1D8)
#define MCSPI2_SOMI         (SYSTEM_CONTROL_MODULE_BASE + 0x1DA)
#define MCSPI2_CS0          (SYSTEM_CONTROL_MODULE_BASE + 0x1DC)
#define MCSPI2_CS1          (SYSTEM_CONTROL_MODULE_BASE + 0x1DE)
#define SYS_NIRQ            (SYSTEM_CONTROL_MODULE_BASE + 0x1E0)
#define SYS_CLKOUT2         (SYSTEM_CONTROL_MODULE_BASE + 0x1E2)
#define ETK_CLK             (SYSTEM_CONTROL_MODULE_BASE + 0x5D8)
#define ETK_CTL             (SYSTEM_CONTROL_MODULE_BASE + 0x5DA)
#define ETK_D0              (SYSTEM_CONTROL_MODULE_BASE + 0x5DC)
#define ETK_D1              (SYSTEM_CONTROL_MODULE_BASE + 0x5DE)
#define ETK_D2              (SYSTEM_CONTROL_MODULE_BASE + 0x5E0)
#define ETK_D3              (SYSTEM_CONTROL_MODULE_BASE + 0x5E2)
#define ETK_D4              (SYSTEM_CONTROL_MODULE_BASE + 0x5E4)
#define ETK_D5              (SYSTEM_CONTROL_MODULE_BASE + 0x5E6)
#define ETK_D6              (SYSTEM_CONTROL_MODULE_BASE + 0x5E8)
#define ETK_D7              (SYSTEM_CONTROL_MODULE_BASE + 0x5EA)
#define ETK_D8              (SYSTEM_CONTROL_MODULE_BASE + 0x5EC)
#define ETK_D9              (SYSTEM_CONTROL_MODULE_BASE + 0x5EE)
#define ETK_D10             (SYSTEM_CONTROL_MODULE_BASE + 0x5F0)
#define ETK_D11             (SYSTEM_CONTROL_MODULE_BASE + 0x5F2)
#define ETK_D12             (SYSTEM_CONTROL_MODULE_BASE + 0x5F4)
#define ETK_D13             (SYSTEM_CONTROL_MODULE_BASE + 0x5F6)
#define ETK_D14             (SYSTEM_CONTROL_MODULE_BASE + 0x5F8)
#define ETK_D15             (SYSTEM_CONTROL_MODULE_BASE + 0x5FA)

//Mux modes
#define MUXMODE0            (0x0UL)
#define MUXMODE1            (0x1UL)
#define MUXMODE2            (0x2UL)
#define MUXMODE3            (0x3UL)
#define MUXMODE4            (0x4UL)
#define MUXMODE5            (0x5UL)
#define MUXMODE6            (0x6UL)
#define MUXMODE7            (0x7UL)

//Pad configuration register.
#define PAD_CONFIG_MASK      (0xFFFFUL)
#define MUXMODE_OFFSET       0
#define MUXMODE_MASK         (0x7UL << MUXMODE_OFFSET)
#define PULL_CONFIG_OFFSET   3
#define PULL_CONFIG_MASK     (0x3UL << PULL_CONFIG_OFFSET)
#define INPUTENABLE_OFFSET   8
#define INPUTENABLE_MASK     (0x1UL << INPUTENABLE_OFFSET)
#define OFFMODE_VALUE_OFFSET 9
#define OFFMODE_VALUE_MASK   (0x1FUL << OFFMODE_VALUE_OFFSET)
#define WAKEUP_OFFSET        14
#define WAKEUP_MASK          (0x2UL << WAKEUP_OFFSET)

#define PULL_DOWN_SELECTED   ((0x0UL << 1) | BIT0)
#define PULL_UP_SELECTED     (BIT1 | BIT0)
#define PULL_DISABLED        (0x0UL << 0)

#define OUTPUT               (0x0UL) //Pin is configured in output only mode.
#define INPUT                (0x1UL) //Pin is configured in bi-directional mode.

typedef struct {
  UINTN   Pin;
  UINTN   MuxMode;
  UINTN   PullConfig;
  UINTN   InputEnable;
} PAD_CONFIGURATION;

#endif //__OMAP3530_PAD_CONFIGURATION_H__
