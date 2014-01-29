/*
 * Freescale GPMI NFC NAND Flash Driver
 *
 * Copyright 2008-2010 Freescale Semiconductor, Inc.
 * Copyright 2008 Embedded Alley Solutions, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __GPMI_NFC_GPMI_REGS_H
#define __GPMI_NFC_GPMI_REGS_H

/*============================================================================*/

#define HW_GPMI_CTRL0      (0x00000000)
#define HW_GPMI_CTRL0_SET  (0x00000004)
#define HW_GPMI_CTRL0_CLR  (0x00000008)
#define HW_GPMI_CTRL0_TOG  (0x0000000c)

#define BM_GPMI_CTRL0_SFTRST	     0x80000000
#define BV_GPMI_CTRL0_SFTRST__RUN    0x0
#define BV_GPMI_CTRL0_SFTRST__RESET  0x1
#define BM_GPMI_CTRL0_CLKGATE           0x40000000
#define BV_GPMI_CTRL0_CLKGATE__RUN      0x0
#define BV_GPMI_CTRL0_CLKGATE__NO_CLKS  0x1
#define BM_GPMI_CTRL0_RUN        0x20000000
#define BV_GPMI_CTRL0_RUN__IDLE  0x0
#define BV_GPMI_CTRL0_RUN__BUSY  0x1
#define BM_GPMI_CTRL0_DEV_IRQ_EN     0x10000000
#define BM_GPMI_CTRL0_TIMEOUT_IRQ_EN 0x08000000
#define BM_GPMI_CTRL0_UDMA           0x04000000
#define BV_GPMI_CTRL0_UDMA__DISABLED 0x0
#define BV_GPMI_CTRL0_UDMA__ENABLED  0x1
#define BP_GPMI_CTRL0_COMMAND_MODE    24
#define BM_GPMI_CTRL0_COMMAND_MODE    0x03000000
#define BF_GPMI_CTRL0_COMMAND_MODE(v) \
	(((v) << 24) & BM_GPMI_CTRL0_COMMAND_MODE)
#define BV_GPMI_CTRL0_COMMAND_MODE__WRITE            0x0
#define BV_GPMI_CTRL0_COMMAND_MODE__READ             0x1
#define BV_GPMI_CTRL0_COMMAND_MODE__READ_AND_COMPARE 0x2
#define BV_GPMI_CTRL0_COMMAND_MODE__WAIT_FOR_READY   0x3
#define BM_GPMI_CTRL0_WORD_LENGTH	   0x00800000
#define BV_GPMI_CTRL0_WORD_LENGTH__16_BIT  0x0
#define BV_GPMI_CTRL0_WORD_LENGTH__8_BIT   0x1
#define BM_GPMI_CTRL0_LOCK_CS            0x00400000
#define BV_GPMI_CTRL0_LOCK_CS__DISABLED  0x0
#define BV_GPMI_CTRL0_LOCK_CS__ENABLED   0x1
#define BP_GPMI_CTRL0_CS     20
#define BM_GPMI_CTRL0_CS     0x00300000
#define BF_GPMI_CTRL0_CS(v)  (((v) << 20) & BM_GPMI_CTRL0_CS)
#define BP_GPMI_CTRL0_ADDRESS	  17
#define BM_GPMI_CTRL0_ADDRESS	  0x000E0000
#define BF_GPMI_CTRL0_ADDRESS(v)  (((v) << 17) & BM_GPMI_CTRL0_ADDRESS)
#define BV_GPMI_CTRL0_ADDRESS__NAND_DATA  0x0
#define BV_GPMI_CTRL0_ADDRESS__NAND_CLE   0x1
#define BV_GPMI_CTRL0_ADDRESS__NAND_ALE   0x2
#define BM_GPMI_CTRL0_ADDRESS_INCREMENT	  0x00010000
#define BV_GPMI_CTRL0_ADDRESS_INCREMENT__DISABLED  0x0
#define BV_GPMI_CTRL0_ADDRESS_INCREMENT__ENABLED   0x1
#define BP_GPMI_CTRL0_XFER_COUNT    0
#define BM_GPMI_CTRL0_XFER_COUNT    0x0000FFFF
#define BF_GPMI_CTRL0_XFER_COUNT(v) \
	(((v) << 0) & BM_GPMI_CTRL0_XFER_COUNT)

/*============================================================================*/

#define HW_GPMI_COMPARE  (0x00000010)

#define BP_GPMI_COMPARE_MASK	 16
#define BM_GPMI_COMPARE_MASK	 0xFFFF0000
#define BF_GPMI_COMPARE_MASK(v)  (((v) << 16) & BM_GPMI_COMPARE_MASK)
#define BP_GPMI_COMPARE_REFERENCE    0
#define BM_GPMI_COMPARE_REFERENCE    0x0000FFFF
#define BF_GPMI_COMPARE_REFERENCE(v) \
	(((v) << 0) & BM_GPMI_COMPARE_REFERENCE)

/*============================================================================*/

#define HW_GPMI_ECCCTRL  (0x00000020)
#define HW_GPMI_ECCCTRL_SET  (0x00000024)
#define HW_GPMI_ECCCTRL_CLR  (0x00000028)
#define HW_GPMI_ECCCTRL_TOG  (0x0000002c)

#define BP_GPMI_ECCCTRL_HANDLE	   16
#define BM_GPMI_ECCCTRL_HANDLE	   0xFFFF0000
#define BF_GPMI_ECCCTRL_HANDLE(v)  (((v) << 16) & BM_GPMI_ECCCTRL_HANDLE)
#define BM_GPMI_ECCCTRL_RSVD2  0x00008000
#define BP_GPMI_ECCCTRL_ECC_CMD  13
#define BM_GPMI_ECCCTRL_ECC_CMD  0x00006000
#define BF_GPMI_ECCCTRL_ECC_CMD(v) (((v) << 13) & BM_GPMI_ECCCTRL_ECC_CMD)
#define BV_GPMI_ECCCTRL_ECC_CMD__DECODE_4_BIT 0x0
#define BV_GPMI_ECCCTRL_ECC_CMD__ENCODE_4_BIT 0x1
#define BV_GPMI_ECCCTRL_ECC_CMD__DECODE_8_BIT 0x2
#define BV_GPMI_ECCCTRL_ECC_CMD__ENCODE_8_BIT 0x3
#define BV_GPMI_ECCCTRL_ECC_CMD__BCH_DECODE 0x0
#define BV_GPMI_ECCCTRL_ECC_CMD__BCH_ENCODE 0x1
#define BM_GPMI_ECCCTRL_ENABLE_ECC           0x00001000
#define BV_GPMI_ECCCTRL_ENABLE_ECC__ENABLE   0x1
#define BV_GPMI_ECCCTRL_ENABLE_ECC__DISABLE  0x0
#define BP_GPMI_ECCCTRL_RSVD1      9
#define BM_GPMI_ECCCTRL_RSVD1      0x00000E00
#define BF_GPMI_ECCCTRL_RSVD1(v)   (((v) << 9) & BM_GPMI_ECCCTRL_RSVD1)
#define BP_GPMI_ECCCTRL_BUFFER_MASK	0
#define BM_GPMI_ECCCTRL_BUFFER_MASK	0x000001FF
#define BF_GPMI_ECCCTRL_BUFFER_MASK(v)  \
		(((v) << 0) & BM_GPMI_ECCCTRL_BUFFER_MASK)
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BCH_AUXONLY 0x100
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BCH_PAGE    0x1FF
#define BV_GPMI_ECCCTRL_BUFFER_MASK__AUXILIARY   0x100
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER7     0x080
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER6     0x040
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER5     0x020
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER4     0x010
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER3     0x008
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER2     0x004
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER1     0x002
#define BV_GPMI_ECCCTRL_BUFFER_MASK__BUFFER0     0x001

/*============================================================================*/

#define HW_GPMI_ECCCOUNT	(0x00000030)

#define BP_GPMI_ECCCOUNT_RSVD2     16
#define BM_GPMI_ECCCOUNT_RSVD2     0xFFFF0000
#define BF_GPMI_ECCCOUNT_RSVD2(v)  (((v) << 16) & BM_GPMI_ECCCOUNT_RSVD2)
#define BP_GPMI_ECCCOUNT_COUNT     0
#define BM_GPMI_ECCCOUNT_COUNT     0x0000FFFF
#define BF_GPMI_ECCCOUNT_COUNT(v)  (((v) << 0) & BM_GPMI_ECCCOUNT_COUNT)

/*============================================================================*/

#define HW_GPMI_PAYLOAD	(0x00000040)

#define BP_GPMI_PAYLOAD_ADDRESS	    2
#define BM_GPMI_PAYLOAD_ADDRESS	    0xFFFFFFFC
#define BF_GPMI_PAYLOAD_ADDRESS(v)  (((v) << 2) & BM_GPMI_PAYLOAD_ADDRESS)
#define BP_GPMI_PAYLOAD_RSVD0     0
#define BM_GPMI_PAYLOAD_RSVD0     0x00000003
#define BF_GPMI_PAYLOAD_RSVD0(v)  (((v) << 0) & BM_GPMI_PAYLOAD_RSVD0)

/*============================================================================*/

#define HW_GPMI_AUXILIARY	(0x00000050)

#define BP_GPMI_AUXILIARY_ADDRESS    2
#define BM_GPMI_AUXILIARY_ADDRESS    0xFFFFFFFC
#define BF_GPMI_AUXILIARY_ADDRESS(v) \
	(((v) << 2) & BM_GPMI_AUXILIARY_ADDRESS)
#define BP_GPMI_AUXILIARY_RSVD0     0
#define BM_GPMI_AUXILIARY_RSVD0     0x00000003
#define BF_GPMI_AUXILIARY_RSVD0(v)  (((v) << 0) & BM_GPMI_AUXILIARY_RSVD0)

/*============================================================================*/

#define HW_GPMI_CTRL1  (0x00000060)
#define HW_GPMI_CTRL1_SET  (0x00000064)
#define HW_GPMI_CTRL1_CLR  (0x00000068)
#define HW_GPMI_CTRL1_TOG  (0x0000006c)

#define BP_GPMI_CTRL1_RSVD2	24
#define BM_GPMI_CTRL1_RSVD2	0xFF000000
#define BF_GPMI_CTRL1_RSVD2(v) \
		(((v) << 24) & BM_GPMI_CTRL1_RSVD2)
#define BM_GPMI_CTRL1_CE3_SEL	0x00800000
#define BM_GPMI_CTRL1_CE2_SEL	0x00400000
#define BM_GPMI_CTRL1_CE1_SEL	0x00200000
#define BM_GPMI_CTRL1_CE0_SEL	0x00100000
#define BM_GPMI_CTRL1_GANGED_RDYBUSY	0x00080000
#define BM_GPMI_CTRL1_GPMI_MODE	0x00000001
#define BP_GPMI_CTRL1_GPMI_MODE	0
#define BM_GPMI_CTRL1_ATA_IRQRDY_POLARITY	0x00000004
#define BM_GPMI_CTRL1_DEV_RESET	0x00000008
#define BM_GPMI_CTRL1_TIMEOUT_IRQ	0x00000200
#define BM_GPMI_CTRL1_DEV_IRQ	0x00000400
#define BM_GPMI_CTRL1_RDN_DELAY	0x0000F000
#define BP_GPMI_CTRL1_RDN_DELAY	12
#define BM_GPMI_CTRL1_BCH_MODE	0x00040000
#define BP_GPMI_CTRL1_DLL_ENABLE	17
#define BM_GPMI_CTRL1_DLL_ENABLE	0x00020000
#define BP_GPMI_CTRL1_HALF_PERIOD	16
#define BM_GPMI_CTRL1_HALF_PERIOD	0x00010000
#define BP_GPMI_CTRL1_RDN_DELAY	12
#define BM_GPMI_CTRL1_RDN_DELAY	0x0000F000
#define BF_GPMI_CTRL1_RDN_DELAY(v)  \
		(((v) << 12) & BM_GPMI_CTRL1_RDN_DELAY)
#define BM_GPMI_CTRL1_DMA2ECC_MODE	0x00000800
#define BM_GPMI_CTRL1_DEV_IRQ	0x00000400
#define BM_GPMI_CTRL1_TIMEOUT_IRQ	0x00000200
#define BM_GPMI_CTRL1_BURST_EN	0x00000100
#define BM_GPMI_CTRL1_ABORT_WAIT_FOR_READY3	0x00000080
#define BM_GPMI_CTRL1_ABORT_WAIT_FOR_READY2	0x00000040
#define BM_GPMI_CTRL1_ABORT_WAIT_FOR_READY1	0x00000020
#define BM_GPMI_CTRL1_ABORT_WAIT_FOR_READY0	0x00000010
#define BM_GPMI_CTRL1_DEV_RESET	0x00000008
#define BV_GPMI_CTRL1_DEV_RESET__ENABLED  0x0
#define BV_GPMI_CTRL1_DEV_RESET__DISABLED 0x1
#define BM_GPMI_CTRL1_ATA_IRQRDY_POLARITY	0x00000004
#define BV_GPMI_CTRL1_ATA_IRQRDY_POLARITY__ACTIVELOW  0x0
#define BV_GPMI_CTRL1_ATA_IRQRDY_POLARITY__ACTIVEHIGH 0x1
#define BM_GPMI_CTRL1_CAMERA_MODE	0x00000002
#define BM_GPMI_CTRL1_GPMI_MODE	0x00000001
#define BV_GPMI_CTRL1_GPMI_MODE__NAND 0x0
#define BV_GPMI_CTRL1_GPMI_MODE__ATA  0x1

/*============================================================================*/

#define HW_GPMI_TIMING0	(0x00000070)

#define BP_GPMI_TIMING0_RSVD1	24
#define BM_GPMI_TIMING0_RSVD1	0xFF000000
#define BF_GPMI_TIMING0_RSVD1(v) \
		(((v) << 24) & BM_GPMI_TIMING0_RSVD1)
#define BP_GPMI_TIMING0_ADDRESS_SETUP	16
#define BM_GPMI_TIMING0_ADDRESS_SETUP	0x00FF0000
#define BF_GPMI_TIMING0_ADDRESS_SETUP(v)  \
		(((v) << 16) & BM_GPMI_TIMING0_ADDRESS_SETUP)
#define BP_GPMI_TIMING0_DATA_HOLD	8
#define BM_GPMI_TIMING0_DATA_HOLD	0x0000FF00
#define BF_GPMI_TIMING0_DATA_HOLD(v)  \
		(((v) << 8) & BM_GPMI_TIMING0_DATA_HOLD)
#define BP_GPMI_TIMING0_DATA_SETUP	0
#define BM_GPMI_TIMING0_DATA_SETUP	0x000000FF
#define BF_GPMI_TIMING0_DATA_SETUP(v)  \
		(((v) << 0) & BM_GPMI_TIMING0_DATA_SETUP)

/*============================================================================*/

#define HW_GPMI_TIMING1	(0x00000080)

#define BP_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT	16
#define BM_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT	0xFFFF0000
#define BF_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT(v) \
		(((v) << 16) & BM_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT)
#define BP_GPMI_TIMING1_RSVD1	0
#define BM_GPMI_TIMING1_RSVD1	0x0000FFFF
#define BF_GPMI_TIMING1_RSVD1(v)  \
		(((v) << 0) & BM_GPMI_TIMING1_RSVD1)

/*============================================================================*/

#define HW_GPMI_TIMING2	(0x00000090)

#define BP_GPMI_TIMING2_UDMA_TRP	24
#define BM_GPMI_TIMING2_UDMA_TRP	0xFF000000
#define BF_GPMI_TIMING2_UDMA_TRP(v) \
		(((v) << 24) & BM_GPMI_TIMING2_UDMA_TRP)
#define BP_GPMI_TIMING2_UDMA_ENV	16
#define BM_GPMI_TIMING2_UDMA_ENV	0x00FF0000
#define BF_GPMI_TIMING2_UDMA_ENV(v)  \
		(((v) << 16) & BM_GPMI_TIMING2_UDMA_ENV)
#define BP_GPMI_TIMING2_UDMA_HOLD	8
#define BM_GPMI_TIMING2_UDMA_HOLD	0x0000FF00
#define BF_GPMI_TIMING2_UDMA_HOLD(v)  \
		(((v) << 8) & BM_GPMI_TIMING2_UDMA_HOLD)
#define BP_GPMI_TIMING2_UDMA_SETUP	0
#define BM_GPMI_TIMING2_UDMA_SETUP	0x000000FF
#define BF_GPMI_TIMING2_UDMA_SETUP(v)  \
		(((v) << 0) & BM_GPMI_TIMING2_UDMA_SETUP)

/*============================================================================*/

#define HW_GPMI_DATA	(0x000000a0)

#define BP_GPMI_DATA_DATA	0
#define BM_GPMI_DATA_DATA	0xFFFFFFFF
#define BF_GPMI_DATA_DATA(v)	(v)

/*============================================================================*/

#define HW_GPMI_STAT	(0x000000b0)

#define BM_GPMI_STAT_PRESENT	0x80000000
#define BV_GPMI_STAT_PRESENT__UNAVAILABLE 0x0
#define BV_GPMI_STAT_PRESENT__AVAILABLE   0x1
#define BP_GPMI_STAT_RSVD1	12
#define BM_GPMI_STAT_RSVD1	0x7FFFF000
#define BF_GPMI_STAT_RSVD1(v)  \
		(((v) << 12) & BM_GPMI_STAT_RSVD1)
#define BP_GPMI_STAT_RDY_TIMEOUT	8
#define BM_GPMI_STAT_RDY_TIMEOUT	0x00000F00
#define BF_GPMI_STAT_RDY_TIMEOUT(v)  \
		(((v) << 8) & BM_GPMI_STAT_RDY_TIMEOUT)
#define BM_GPMI_STAT_ATA_IRQ	0x00000080
#define BM_GPMI_STAT_INVALID_BUFFER_MASK	0x00000040
#define BM_GPMI_STAT_FIFO_EMPTY	0x00000020
#define BV_GPMI_STAT_FIFO_EMPTY__NOT_EMPTY 0x0
#define BV_GPMI_STAT_FIFO_EMPTY__EMPTY     0x1
#define BM_GPMI_STAT_FIFO_FULL	0x00000010
#define BV_GPMI_STAT_FIFO_FULL__NOT_FULL 0x0
#define BV_GPMI_STAT_FIFO_FULL__FULL     0x1
#define BM_GPMI_STAT_DEV3_ERROR	0x00000008
#define BM_GPMI_STAT_DEV2_ERROR	0x00000004
#define BM_GPMI_STAT_DEV1_ERROR	0x00000002
#define BM_GPMI_STAT_DEERROR	0x00000001

/*============================================================================*/

#define HW_GPMI_DEBUG	(0x000000c0)

#define BM_GPMI_DEBUG_READY3	0x80000000
#define BM_GPMI_DEBUG_READY2	0x40000000
#define BM_GPMI_DEBUG_READY1	0x20000000
#define BM_GPMI_DEBUG_READY0	0x10000000
#define BM_GPMI_DEBUG_WAIT_FOR_READY_END3	0x08000000
#define BM_GPMI_DEBUG_WAIT_FOR_READY_END2	0x04000000
#define BM_GPMI_DEBUG_WAIT_FOR_READY_END1	0x02000000
#define BM_GPMI_DEBUG_WAIT_FOR_READY_END0	0x01000000
#define BM_GPMI_DEBUG_SENSE3	0x00800000
#define BM_GPMI_DEBUG_SENSE2	0x00400000
#define BM_GPMI_DEBUG_SENSE1	0x00200000
#define BM_GPMI_DEBUG_SENSE0	0x00100000
#define BM_GPMI_DEBUG_DMAREQ3	0x00080000
#define BM_GPMI_DEBUG_DMAREQ2	0x00040000
#define BM_GPMI_DEBUG_DMAREQ1	0x00020000
#define BM_GPMI_DEBUG_DMAREQ0	0x00010000
#define BP_GPMI_DEBUG_CMD_END	12
#define BM_GPMI_DEBUG_CMD_END	0x0000F000
#define BF_GPMI_DEBUG_CMD_END(v)  \
		(((v) << 12) & BM_GPMI_DEBUG_CMD_END)
#define BP_GPMI_DEBUG_UDMA_STATE	8
#define BM_GPMI_DEBUG_UDMA_STATE	0x00000F00
#define BF_GPMI_DEBUG_UDMA_STATE(v)  \
		(((v) << 8) & BM_GPMI_DEBUG_UDMA_STATE)
#define BM_GPMI_DEBUG_BUSY	0x00000080
#define BV_GPMI_DEBUG_BUSY__DISABLED 0x0
#define BV_GPMI_DEBUG_BUSY__ENABLED  0x1
#define BP_GPMI_DEBUG_PIN_STATE	4
#define BM_GPMI_DEBUG_PIN_STATE	0x00000070
#define BF_GPMI_DEBUG_PIN_STATE(v)  \
		(((v) << 4) & BM_GPMI_DEBUG_PIN_STATE)
#define BV_GPMI_DEBUG_PIN_STATE__PSM_IDLE   0x0
#define BV_GPMI_DEBUG_PIN_STATE__PSM_BYTCNT 0x1
#define BV_GPMI_DEBUG_PIN_STATE__PSM_ADDR   0x2
#define BV_GPMI_DEBUG_PIN_STATE__PSM_STALL  0x3
#define BV_GPMI_DEBUG_PIN_STATE__PSM_STROBE 0x4
#define BV_GPMI_DEBUG_PIN_STATE__PSM_ATARDY 0x5
#define BV_GPMI_DEBUG_PIN_STATE__PSM_DHOLD  0x6
#define BV_GPMI_DEBUG_PIN_STATE__PSM_DONE   0x7
#define BP_GPMI_DEBUG_MAIN_STATE	0
#define BM_GPMI_DEBUG_MAIN_STATE	0x0000000F
#define BF_GPMI_DEBUG_MAIN_STATE(v)  \
		(((v) << 0) & BM_GPMI_DEBUG_MAIN_STATE)
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_IDLE   0x0
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_BYTCNT 0x1
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_WAITFE 0x2
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_WAITFR 0x3
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_DMAREQ 0x4
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_DMAACK 0x5
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_WAITFF 0x6
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_LDFIFO 0x7
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_LDDMAR 0x8
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_RDCMP  0x9
#define BV_GPMI_DEBUG_MAIN_STATE__MSM_DONE   0xA

/*============================================================================*/

#define HW_GPMI_VERSION	(0x000000d0)

#define BP_GPMI_VERSION_MAJOR     24
#define BM_GPMI_VERSION_MAJOR     0xFF000000
#define BF_GPMI_VERSION_MAJOR(v)  (((v) << 24) & BM_GPMI_VERSION_MAJOR)
#define BP_GPMI_VERSION_MINOR     16
#define BM_GPMI_VERSION_MINOR     0x00FF0000
#define BF_GPMI_VERSION_MINOR(v)  (((v) << 16) & BM_GPMI_VERSION_MINOR)
#define BP_GPMI_VERSION_STEP      0
#define BM_GPMI_VERSION_STEP      0x0000FFFF
#define BF_GPMI_VERSION_STEP(v)   (((v) << 0) & BM_GPMI_VERSION_STEP)

/*============================================================================*/

#define HW_GPMI_DEBUG2       (0x000000e0)

#define BP_GPMI_DEBUG2_RSVD1	16
#define BM_GPMI_DEBUG2_RSVD1	0xFFFF0000
#define BF_GPMI_DEBUG2_RSVD1(v)	(((v) << 16) & BM_GPMI_DEBUG2_RSVD1)
#define BP_GPMI_DEBUG2_SYND2GPMI_BE	12
#define BM_GPMI_DEBUG2_SYND2GPMI_BE	0x0000F000
#define BF_GPMI_DEBUG2_SYND2GPMI_BE(v)  \
		(((v) << 12) & BM_GPMI_DEBUG2_SYND2GPMI_BE)
#define BM_GPMI_DEBUG2_GPMI2SYND_VALID	0x00000800
#define BM_GPMI_DEBUG2_GPMI2SYND_READY	0x00000400
#define BM_GPMI_DEBUG2_SYND2GPMI_VALID	0x00000200
#define BM_GPMI_DEBUG2_SYND2GPMI_READY	0x00000100
#define BM_GPMI_DEBUG2_VIEW_DELAYED_RDN	0x00000080
#define BM_GPMI_DEBUG2_UPDATE_WINDOW	0x00000040
#define BP_GPMI_DEBUG2_RDN_TAP     0
#define BM_GPMI_DEBUG2_RDN_TAP     0x0000003F
#define BF_GPMI_DEBUG2_RDN_TAP(v)  (((v) << 0) & BM_GPMI_DEBUG2_RDN_TAP)

/*============================================================================*/

#define HW_GPMI_DEBUG3       (0x000000f0)

#define BP_GPMI_DEBUG3_APB_WORD_CNTR	16
#define BM_GPMI_DEBUG3_APB_WORD_CNTR	0xFFFF0000
#define BF_GPMI_DEBUG3_APB_WORD_CNTR(v) \
		(((v) << 16) & BM_GPMI_DEBUG3_APB_WORD_CNTR)
#define BP_GPMI_DEBUG3_DEV_WORD_CNTR	0
#define BM_GPMI_DEBUG3_DEV_WORD_CNTR	0x0000FFFF
#define BF_GPMI_DEBUG3_DEV_WORD_CNTR(v)  \
		(((v) << 0) & BM_GPMI_DEBUG3_DEV_WORD_CNTR)

/*============================================================================*/
#endif
