/*
 *  linux/drivers/mmc/core/mmc.c
 *
 *  Copyright (C) 2003-2004 Russell King, All Rights Reserved.
 *  Copyright (C) 2005-2007 Pierre Ossman, All Rights Reserved.
 *  MMCv4 support Copyright (C) 2006 Philip Langdale, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/slab.h>

#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>

#include "core.h"
#include "bus.h"
#include "mmc_ops.h"

static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static int mmc_decode_cid(struct mmc_card *card)
{
	u32 *resp = card->raw_cid;

	/*
	 * The selection of the format here is based upon published
	 * specs from sandisk and from what people have reported.
	 */
	switch (card->csd.mmca_vsn) {
	case 0: /* MMC v1.0 - v1.2 */
	case 1: /* MMC v1.4 */
		card->cid.manfid	= UNSTUFF_BITS(resp, 104, 24);
		card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		card->cid.prod_name[6]	= UNSTUFF_BITS(resp, 48, 8);
		card->cid.hwrev		= UNSTUFF_BITS(resp, 44, 4);
		card->cid.fwrev		= UNSTUFF_BITS(resp, 40, 4);
		card->cid.serial	= UNSTUFF_BITS(resp, 16, 24);
		card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
		card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	case 2: /* MMC v2.0 - v2.2 */
	case 3: /* MMC v3.1 - v3.3 */
	case 4: /* MMC v4 */
		card->cid.manfid	= UNSTUFF_BITS(resp, 120, 8);
		card->cid.oemid		= UNSTUFF_BITS(resp, 104, 16);
		card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		card->cid.serial	= UNSTUFF_BITS(resp, 16, 32);
		card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
		card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	default:
		printk(KERN_ERR "%s: card has unknown MMCA version %d\n",
			mmc_hostname(card->host), card->csd.mmca_vsn);
		return -EINVAL;
	}

	return 0;
}

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static int mmc_decode_csd(struct mmc_card *card)
{
	struct mmc_csd *csd = &card->csd;
	unsigned int e, m, csd_struct;
	u32 *resp = card->raw_csd;

	/*
	 * We only understand CSD structure v1.1 and v1.2.
	 * v1.2 has extra information in bits 15, 11 and 10.
	 */
	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	if (csd_struct < 1 || csd_struct > 3) {
		printk(KERN_ERR "%s: unrecognised CSD structure version %d\n",
			mmc_hostname(card->host), csd_struct);
		return -EINVAL;
	}

	csd->mmca_vsn	 = UNSTUFF_BITS(resp, 122, 4);
	m = UNSTUFF_BITS(resp, 115, 4);
	e = UNSTUFF_BITS(resp, 112, 3);
	csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
	csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

	m = UNSTUFF_BITS(resp, 99, 4);
	e = UNSTUFF_BITS(resp, 96, 3);
	csd->max_dtr	  = tran_exp[e] * tran_mant[m];
	csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

	e = UNSTUFF_BITS(resp, 47, 3);
	m = UNSTUFF_BITS(resp, 62, 12);
	csd->capacity	  = (1 + m) << (e + 2);

	csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
	csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
	csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
	csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
	csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
	csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
	csd->write_partial = UNSTUFF_BITS(resp, 21, 1);

	return 0;
}

/*
 * Read and decode extended CSD.
 */
static int mmc_read_ext_csd(struct mmc_card *card)
{
	int err;
	u8 *ext_csd;

	BUG_ON(!card);

	if (card->csd.mmca_vsn < CSD_SPEC_VER_4)
		return 0;

	/*
	 * As the ext_csd is so large and mostly unused, we don't store the
	 * raw block in mmc_card.
	 */
	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		printk(KERN_ERR "%s: could not allocate a buffer to "
			"receive the ext_csd.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}

	err = mmc_send_ext_csd(card, ext_csd);
	if (err) {
		/* If the host or the card can't do the switch,
		 * fail more gracefully. */
		if ((err != -EINVAL)
		 && (err != -ENOSYS)
		 && (err != -EFAULT))
			goto out;

		/*
		 * High capacity cards should have this "magic" size
		 * stored in their CSD.
		 */
		if (card->csd.capacity == (4096 * 512)) {
			printk(KERN_ERR "%s: unable to read EXT_CSD "
				"on a possible high capacity card. "
				"Card will be ignored.\n",
				mmc_hostname(card->host));
		} else {
			printk(KERN_WARNING "%s: unable to read "
				"EXT_CSD, performance might "
				"suffer.\n",
				mmc_hostname(card->host));
			err = 0;
		}

		goto out;
	}

	card->ext_csd.rev = ext_csd[EXT_CSD_REV];
#if 0	// skip ext_csd version check. Joseph 20130422
	if (card->ext_csd.rev > 5) {
		printk(KERN_ERR "%s: unrecognised EXT_CSD structure "
			"version %d\n", mmc_hostname(card->host),
			card->ext_csd.rev);
		err = -EINVAL;
		goto out;
	}
#endif

	if (card->ext_csd.rev >= 2) {
		card->ext_csd.sectors =
			ext_csd[EXT_CSD_SEC_CNT + 0] << 0 |
			ext_csd[EXT_CSD_SEC_CNT + 1] << 8 |
			ext_csd[EXT_CSD_SEC_CNT + 2] << 16 |
			ext_csd[EXT_CSD_SEC_CNT + 3] << 24;

		/* Cards with density > 2GiB are sector addressed */
		if (card->ext_csd.sectors > (2u * 1024 * 1024 * 1024) / 512)
			mmc_card_set_blockaddr(card);
	}

	card->ext_csd.boot_info = ext_csd[EXT_CSD_BOOT_INFO];
	card->ext_csd.boot_size_mult = ext_csd[EXT_CSD_BOOT_SIZE_MULT];
	card->ext_csd.boot_config = ext_csd[EXT_CSD_BOOT_CONFIG];
	card->ext_csd.boot_bus_width = ext_csd[EXT_CSD_BOOT_BUS_WIDTH];
	card->ext_csd.card_type = ext_csd[EXT_CSD_CARD_TYPE];

	switch (ext_csd[EXT_CSD_CARD_TYPE] & EXT_CSD_CARD_TYPE_MASK) {
	case EXT_CSD_CARD_TYPE_52 | EXT_CSD_CARD_TYPE_26:
		card->ext_csd.hs_max_dtr = 52000000;
		break;
	case EXT_CSD_CARD_TYPE_26:
		card->ext_csd.hs_max_dtr = 26000000;
		break;
	default:
		/* MMC v4 spec says this cannot happen */
		printk(KERN_WARNING "%s: card is mmc v4 but doesn't "
			"support any high-speed modes.\n",
			mmc_hostname(card->host));
		printk(KERN_WARNING "%s: card type is 0x%x\n",
			mmc_hostname(card->host), ext_csd[EXT_CSD_CARD_TYPE]);
		goto out;
	}

	if (card->ext_csd.rev >= 3) {
		u8 sa_shift = ext_csd[EXT_CSD_S_A_TIMEOUT];

		/* Sleep / awake timeout in 100ns units */
		if (sa_shift > 0 && sa_shift <= 0x17)
			card->ext_csd.sa_timeout =
					1 << ext_csd[EXT_CSD_S_A_TIMEOUT];
	}

out:
	kfree(ext_csd);

	return err;
}

/* configure the boot partitions */
static ssize_t
setup_boot_partitions(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	int err, busy = 0;
	u32 part, new_part;
	u8 *ext_csd, boot_config;
	struct mmc_command cmd;
	struct mmc_card *card = container_of(dev, struct mmc_card, dev);

	BUG_ON(!card);

	sscanf(buf, "%d\n", &part);

	if (card->csd.mmca_vsn < CSD_SPEC_VER_4) {
		printk(KERN_ERR "%s: invalid mmc version"
			" mmc version is below version 4!)\n",
			mmc_hostname(card->host));
		return -EINVAL;
	}

	/* it's a normal SD/MMC but user request to configure boot partition */
	if (card->ext_csd.boot_size_mult <= 0) {
		printk(KERN_ERR "%s: this is a normal SD/MMC card"
			" but you request to access boot partition!\n",
			mmc_hostname(card->host));
		return -EINVAL;
	}

	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		printk(KERN_ERR "%s: could not allocate a buffer to "
			"receive the ext_csd.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}

	mmc_claim_host(card->host);
	err = mmc_send_ext_csd(card, ext_csd);
	if (err) {
		printk(KERN_ERR "%s: unable to read EXT_CSD.\n",
			mmc_hostname(card->host));
		goto err_rtn;
	}

	/* enable the boot partition in boot mode */
	/* boot enable be -
	 * 0x00 - disable boot enable.
	 * 0x08 - boot partition 1 is enabled for boot.
	 * 0x10 - boot partition 2 is enabled for boot.
	 * 0x38 - User area is enabled for boot.
	 */
	switch (part & EXT_CSD_BOOT_PARTITION_ENABLE_MASK) {
	case 0:
		boot_config = (ext_csd[EXT_CSD_BOOT_CONFIG]
				& ~EXT_CSD_BOOT_PARTITION_ENABLE_MASK
				& ~EXT_CSD_BOOT_ACK_ENABLE);
		break;
	case EXT_CSD_BOOT_PARTITION_PART1:
		boot_config = ((ext_csd[EXT_CSD_BOOT_CONFIG]
				& ~EXT_CSD_BOOT_PARTITION_ENABLE_MASK)
				| EXT_CSD_BOOT_PARTITION_PART1
				| EXT_CSD_BOOT_ACK_ENABLE);
		break;
	case EXT_CSD_BOOT_PARTITION_PART2:
		boot_config = ((ext_csd[EXT_CSD_BOOT_CONFIG]
				& ~EXT_CSD_BOOT_PARTITION_ENABLE_MASK)
				| EXT_CSD_BOOT_PARTITION_PART2
				| EXT_CSD_BOOT_ACK_ENABLE);
		break;
	case EXT_CSD_BOOT_PARTITION_ENABLE_MASK:
		boot_config = ((ext_csd[EXT_CSD_BOOT_CONFIG]
				| EXT_CSD_BOOT_PARTITION_ENABLE_MASK)
				& ~EXT_CSD_BOOT_ACK_ENABLE);
		break;
	default:
		printk(KERN_ERR "%s: wrong boot config parameter"
			" 00 (disable boot), 08 (enable boot1),"
			"16 (enable boot2), 56 (User area)\n",
			mmc_hostname(card->host));
		err = -EINVAL;
		goto err_rtn;
	}

	/* switch the partitions that used to be accessed in OS layer */
	/* partition must be -
	 * 0 - user area
	 * 1 - boot partition 1
	 * 2 - boot partition 2
	 */
	if ((part & EXT_CSD_BOOT_PARTITION_ACCESS_MASK) > 2) {
		printk(KERN_ERR "%s: wrong partition id"
			" 0 (user area), 1 (boot1), 2 (boot2)\n",
			mmc_hostname(card->host));
		err = -EINVAL;
		goto err_rtn;
	}

	/* Send SWITCH command to change partition for access */
	boot_config &= ~EXT_CSD_BOOT_PARTITION_ACCESS_MASK;
	boot_config |= (part & EXT_CSD_BOOT_PARTITION_ACCESS_MASK);
	err = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL,
			EXT_CSD_BOOT_CONFIG, boot_config);
	if (err) {
		printk(KERN_ERR "%s: fail to send SWITCH command"
				" to card to swich partition for access!\n",
			mmc_hostname(card->host));
		goto err_rtn;
	}

	/* waiting for the card to finish the busy state */
	do {
		memset(&cmd, 0, sizeof(struct mmc_command));

		cmd.opcode = MMC_SEND_STATUS;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

		err = mmc_wait_for_cmd(card->host, &cmd, 0);
		if (err || busy > 100) {
			printk(KERN_ERR "%s: failed to wait for"
				"the busy state to end.\n",
				mmc_hostname(card->host));
			break;
		}

		if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) {
			printk(KERN_INFO "%s: card is in busy state"
				"pls wait for busy state to end.\n",
				mmc_hostname(card->host));
		}
		busy++;
	} while (!(cmd.resp[0] & R1_READY_FOR_DATA));

	/* Now check whether it works */
	err = mmc_send_ext_csd(card, ext_csd);
	if (err) {
		printk(KERN_ERR "%s: %d unable to re-read EXT_CSD.\n",
			mmc_hostname(card->host), err);
		goto err_rtn;
	}

	new_part = ext_csd[EXT_CSD_BOOT_CONFIG] &
		EXT_CSD_BOOT_PARTITION_ACCESS_MASK;
	if ((part & EXT_CSD_BOOT_PARTITION_ACCESS_MASK) != new_part) {
		printk(KERN_ERR "%s: after SWITCH, current part id %d is not"
				" same as requested partition %d!\n",
			mmc_hostname(card->host), new_part, part);
		goto err_rtn;
	}
	card->ext_csd.boot_config = ext_csd[EXT_CSD_BOOT_CONFIG];

err_rtn:
	mmc_release_host(card->host);
	kfree(ext_csd);
	if (err)
		return err;
	else
		return count;
}

/* configure the boot bus */
static ssize_t
setup_boot_bus(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	int err, busy = 0;
	u32 boot_bus, new_bus;
	u8 *ext_csd;
	struct mmc_command cmd;
	struct mmc_card *card = container_of(dev, struct mmc_card, dev);

	BUG_ON(!card);

	sscanf(buf, "%d\n", &boot_bus);

	if (card->csd.mmca_vsn < CSD_SPEC_VER_4) {
		printk(KERN_ERR "%s: invalid mmc version"
			" mmc version is below version 4!)\n",
			mmc_hostname(card->host));
		return -EINVAL;
	}

	/* it's a normal SD/MMC but user request to configure boot bus */
	if (card->ext_csd.boot_size_mult <= 0) {
		printk(KERN_ERR "%s: this is a normal SD/MMC card"
			" but you request to configure boot bus !\n",
			mmc_hostname(card->host));
		return -EINVAL;
	}

	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		printk(KERN_ERR "%s: could not allocate a buffer to "
			"receive the ext_csd.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}

	mmc_claim_host(card->host);
	err = mmc_send_ext_csd(card, ext_csd);
	if (err) {
		printk(KERN_ERR "%s: unable to read EXT_CSD.\n",
			mmc_hostname(card->host));
		goto err_rtn;
	}

	/* Configure the boot bus width when boot partition is enabled */
	if (((boot_bus & EXT_CSD_BOOT_BUS_WIDTH_MODE_MASK) >> 3) > 2
			|| (boot_bus & EXT_CSD_BOOT_BUS_WIDTH_WIDTH_MASK) > 2
			|| (boot_bus & ~EXT_CSD_BOOT_BUS_WIDTH_MASK) > 0) {
		printk(KERN_ERR "%s: Invalid inputs!\n",
			mmc_hostname(card->host));
		err = -EINVAL;
		goto err_rtn;
	}

	err = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL,
			EXT_CSD_BOOT_BUS_WIDTH, boot_bus);
	if (err) {
		printk(KERN_ERR "%s: fail to send SWITCH command to "
				"card to swich partition for access!\n",
			mmc_hostname(card->host));
		goto err_rtn;
	}

	/* waiting for the card to finish the busy state */
	do {
		memset(&cmd, 0, sizeof(struct mmc_command));

		cmd.opcode = MMC_SEND_STATUS;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

		err = mmc_wait_for_cmd(card->host, &cmd, 0);
		if (err || busy > 100) {
			printk(KERN_ERR "%s: failed to wait for"
				"the busy state to end.\n",
				mmc_hostname(card->host));
			break;
		}

		if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) {
			printk(KERN_INFO "%s: card is in busy state"
				"pls wait for busy state to end.\n",
				mmc_hostname(card->host));
		}
		busy++;
	} while (!(cmd.resp[0] & R1_READY_FOR_DATA));

	/* Now check whether it works */
	err = mmc_send_ext_csd(card, ext_csd);
	if (err) {
		printk(KERN_ERR "%s: %d unable to re-read EXT_CSD.\n",
			mmc_hostname(card->host), err);
		goto err_rtn;
	}

	new_bus = ext_csd[EXT_CSD_BOOT_BUS_WIDTH];
	if (boot_bus  != new_bus) {
		printk(KERN_ERR "%s: after SWITCH, current boot bus mode %d"
				" is not same as requested bus mode %d!\n",
			mmc_hostname(card->host), new_bus, boot_bus);
		goto err_rtn;
	}
	card->ext_csd.boot_bus_width = ext_csd[EXT_CSD_BOOT_BUS_WIDTH];

err_rtn:
	mmc_release_host(card->host);
	kfree(ext_csd);
	if (err)
		return err;
	else
		return count;
}

MMC_DEV_ATTR(cid, "%08x%08x%08x%08x\n", card->raw_cid[0], card->raw_cid[1],
	card->raw_cid[2], card->raw_cid[3]);
MMC_DEV_ATTR(csd, "%08x%08x%08x%08x\n", card->raw_csd[0], card->raw_csd[1],
	card->raw_csd[2], card->raw_csd[3]);
MMC_DEV_ATTR(date, "%02d/%04d\n", card->cid.month, card->cid.year);
MMC_DEV_ATTR(fwrev, "0x%x\n", card->cid.fwrev);
MMC_DEV_ATTR(hwrev, "0x%x\n", card->cid.hwrev);
MMC_DEV_ATTR(manfid, "0x%06x\n", card->cid.manfid);
MMC_DEV_ATTR(name, "%s\n", card->cid.prod_name);
MMC_DEV_ATTR(oemid, "0x%04x\n", card->cid.oemid);
MMC_DEV_ATTR(serial, "0x%08x\n", card->cid.serial);

static ssize_t mmc_boot_info_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char *boot_partition[8] = {
		"Device not boot enabled",
		"Boot partition 1 enabled",
		"Boot partition 2 enabled",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"User area enabled for boot"};

	char *boot_partition_access[8] = {
		"No access to boot partition",
		"R/W boot partition 1",
		"R/W boot partition 2",
		"R/W Replay Protected Memory Block (RPMB)",
		"Access to General Purpose partition 1",
		"Access to General Purpose partition 2",
		"Access to General Purpose partition 3",
		"Access to General Purpose partition 4"};

	char *bus_width[4] = {
		"x1 (sdr) or x4 (ddr) bus width in boot operation mode",
		"x4 (sdr/ddr) bus width in boot operation mode",
		"x8 (sdr/ddr) bus width in boot operation mode",
		"Reserved"};

	char *boot_mode[4] = {
	"Use single data rate + backward compatible timings in boot operation",
	"Use single data rate + high speed timings in boot operation mode",
	"Use dual data rate in boot operation",
	"Reserved"};

	int partition;
	int access;
	int width;
	int mode;
	struct mmc_card *card = container_of(dev, struct mmc_card, dev);

	/* read it again because user may change it */
	mmc_claim_host(card->host);
	mmc_read_ext_csd(card);
	mmc_release_host(card->host);

	partition = (card->ext_csd.boot_config >> 3) & 0x7;
	access = card->ext_csd.boot_config & 0x7;
	width =  card->ext_csd.boot_bus_width & 0x3;
	mode = (card->ext_csd.boot_bus_width >> 3) & 0x3;

	return sprintf(buf,
		"boot_info:0x%02x;\n"
		"  ALT_BOOT_MODE:%x - %s\n"
		"  DDR_BOOT_MODE:%x - %s\n"
		"  HS_BOOTMODE:%x - %s\n"
		"boot_size:%04dKB\n"
		"boot_partition:0x%02x;\n"
		"  BOOT_ACK:%x - %s\n"
		"  BOOT_PARTITION-ENABLE: %x - %s\n"
		"  PARTITION_ACCESS:%x - %s\n"
		"boot_bus:0x%02x\n"
		"  BOOT_MODE:%x - %s\n"
		"  RESET_BOOT_BUS_WIDTH:%x - %s\n"
		"  BOOT_BUS_WIDTH:%x - %s\n",

		card->ext_csd.boot_info,
		!!(card->ext_csd.boot_info & 0x1),
		(card->ext_csd.boot_info & 0x1) ?
			"Supports alternate boot method" :
			"Does not support alternate boot method",
		!!(card->ext_csd.boot_info & 0x2),
		(card->ext_csd.boot_info & 0x2) ?
			"Supports alternate dual data rate during boot" :
			"Does not support dual data rate during boot",
		!!(card->ext_csd.boot_info & 0x4),
		(card->ext_csd.boot_info & 0x4) ?
			"Supports high speed timing during boot" :
			"Does not support high speed timing during boot",

		card->ext_csd.boot_size_mult * 128,

		card->ext_csd.boot_config,
		!!(card->ext_csd.boot_config & 0x40),
		(card->ext_csd.boot_config & 0x40) ?
			"Boot acknowledge sent during boot operation" :
			"No boot acknowledge sent",
		partition,
		boot_partition[partition],
		access,
		boot_partition_access[access],

		card->ext_csd.boot_bus_width,
		mode,
		boot_mode[mode],
		!!(card->ext_csd.boot_bus_width & 0x4),
		(card->ext_csd.boot_bus_width & 0x4) ?
		  "Retain boot bus width and boot mode after boot operation" :
		  "Reset bus width to x1, single data rate and backward"
		  "compatible timings after boot operation",
		width,
		bus_width[width]);
}

DEVICE_ATTR(boot_config, S_IWUGO, NULL, setup_boot_partitions);
DEVICE_ATTR(boot_bus_config, S_IWUGO, NULL, setup_boot_bus);
DEVICE_ATTR(boot_info, S_IRUGO, mmc_boot_info_show, NULL);

static struct attribute *mmc_std_attrs[] = {
	&dev_attr_cid.attr,
	&dev_attr_csd.attr,
	&dev_attr_date.attr,
	&dev_attr_fwrev.attr,
	&dev_attr_hwrev.attr,
	&dev_attr_manfid.attr,
	&dev_attr_name.attr,
	&dev_attr_oemid.attr,
	&dev_attr_serial.attr,
	&dev_attr_boot_info.attr,
	&dev_attr_boot_config.attr,
	&dev_attr_boot_bus_config.attr,
	NULL,
};

static struct attribute_group mmc_std_attr_group = {
	.attrs = mmc_std_attrs,
};

static const struct attribute_group *mmc_attr_groups[] = {
	&mmc_std_attr_group,
	NULL,
};

static struct device_type mmc_type = {
	.groups = mmc_attr_groups,
};

/*
 * Handle the detection and initialisation of a card.
 *
 * In the case of a resume, "oldcard" will contain the card
 * we're trying to reinitialise.
 */
static int mmc_init_card(struct mmc_host *host, u32 ocr,
	struct mmc_card *oldcard)
{
	struct mmc_card *card;
	int err;
	u32 cid[4];
	unsigned int max_dtr;

	BUG_ON(!host);
	WARN_ON(!host->claimed);

	/*
	 * Since we're changing the OCR value, we seem to
	 * need to tell some cards to go back to the idle
	 * state.  We wait 1ms to give cards time to
	 * respond.
	 */
	mmc_go_idle(host);

	/* The extra bit indicates that we support high capacity */
	err = mmc_send_op_cond(host, ocr | (1 << 30), NULL);
	if (err)
		goto err;

	/*
	 * For SPI, enable CRC as appropriate.
	 */
	if (mmc_host_is_spi(host)) {
		err = mmc_spi_set_crc(host, use_spi_crc);
		if (err)
			goto err;
	}

	/*
	 * Fetch CID from card.
	 */
	if (mmc_host_is_spi(host))
		err = mmc_send_cid(host, cid);
	else
		err = mmc_all_send_cid(host, cid);
	if (err)
		goto err;

	if (oldcard) {
		if (memcmp(cid, oldcard->raw_cid, sizeof(cid)) != 0) {
			err = -ENOENT;
			goto err;
		}

		card = oldcard;
	} else {
		/*
		 * Allocate card structure.
		 */
		card = mmc_alloc_card(host, &mmc_type);
		if (IS_ERR(card)) {
			err = PTR_ERR(card);
			goto err;
		}

		card->type = MMC_TYPE_MMC;
		card->rca = 1;
		memcpy(card->raw_cid, cid, sizeof(card->raw_cid));
	}

	/*
	 * For native busses:  set card RCA and quit open drain mode.
	 */
	if (!mmc_host_is_spi(host)) {
		err = mmc_set_relative_addr(card);
		if (err)
			goto free_card;

		mmc_set_bus_mode(host, MMC_BUSMODE_PUSHPULL);
	}

	if (!oldcard) {
		/*
		 * Fetch CSD from card.
		 */
		err = mmc_send_csd(card, card->raw_csd);
		if (err)
			goto free_card;

		err = mmc_decode_csd(card);
		if (err)
			goto free_card;
		err = mmc_decode_cid(card);
		if (err)
			goto free_card;
	}

	/*
	 * Select card, as all following commands rely on that.
	 */
	if (!mmc_host_is_spi(host)) {
		err = mmc_select_card(card);
		if (err)
			goto free_card;
	}

	if (!oldcard) {
		/*
		 * Fetch and process extended CSD.
		 */
		err = mmc_read_ext_csd(card);
		if (err)
			goto free_card;
	}

	/*
	 * Activate high speed (if supported)
	 */
	if ((card->ext_csd.hs_max_dtr != 0) &&
		(host->caps & MMC_CAP_MMC_HIGHSPEED)) {
		err = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL,
			EXT_CSD_HS_TIMING, 1);
		if (err && err != -EBADMSG)
			goto free_card;

		if (err) {
			printk(KERN_WARNING "%s: switch to highspeed failed\n",
			       mmc_hostname(card->host));
			err = 0;
		} else {
			mmc_card_set_highspeed(card);
			mmc_set_timing(card->host, MMC_TIMING_MMC_HS);
		}
	}

	/*
	 * Compute bus speed.
	 */
	max_dtr = (unsigned int)-1;

	if (mmc_card_highspeed(card)) {
		if (max_dtr > card->ext_csd.hs_max_dtr)
			max_dtr = card->ext_csd.hs_max_dtr;
	} else if (max_dtr > card->csd.max_dtr) {
		max_dtr = card->csd.max_dtr;
	}

	mmc_set_clock(host, max_dtr);

	/*
	 * Activate wide bus (if supported).
	 */
	if ((card->csd.mmca_vsn >= CSD_SPEC_VER_4) &&
		(host->caps & (MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA
			       | MMC_CAP_DATA_DDR))) {
		unsigned ext_csd_bit, bus_width;

		if ((host->caps & MMC_CAP_8_BIT_DATA) &&
				(host->caps & MMC_CAP_DATA_DDR) &&
				(card->ext_csd.card_type & MMC_DDR_MODE_MASK)) {
			ext_csd_bit = EXT_CSD_BUS_WIDTH_8_DDR;
			bus_width = MMC_BUS_WIDTH_8 | MMC_BUS_WIDTH_DDR;
			mmc_card_set_ddrmode(card);
		} else if ((host->caps & MMC_CAP_4_BIT_DATA) &&
				(host->caps & MMC_CAP_DATA_DDR) &&
				(card->ext_csd.card_type & MMC_DDR_MODE_MASK)) {
			ext_csd_bit = EXT_CSD_BUS_WIDTH_4_DDR;
			bus_width = MMC_BUS_WIDTH_4 | MMC_BUS_WIDTH_DDR;
			mmc_card_set_ddrmode(card);
		} else if (host->caps & MMC_CAP_8_BIT_DATA) {
			ext_csd_bit = EXT_CSD_BUS_WIDTH_8;
			bus_width = MMC_BUS_WIDTH_8;
		} else {
			ext_csd_bit = EXT_CSD_BUS_WIDTH_4;
			bus_width = MMC_BUS_WIDTH_4;
		}

		err = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL,
				 EXT_CSD_BUS_WIDTH, ext_csd_bit);

		if (err && err != -EBADMSG)
			goto free_card;

		if (err) {
			printk(KERN_WARNING "%s: switch to bus width %d "
			       "failed\n", mmc_hostname(card->host),
			       1 << bus_width);
			err = 0;
		} else {
			mmc_set_bus_width(card->host, bus_width);
		}
	}

	if (!oldcard)
		host->card = card;

	return 0;

free_card:
	if (!oldcard)
		mmc_remove_card(card);
err:

	return err;
}

/*
 * Host is being removed. Free up the current card.
 */
static void mmc_remove(struct mmc_host *host)
{
	BUG_ON(!host);
	BUG_ON(!host->card);

	mmc_remove_card(host->card);
	host->card = NULL;
}

/*
 * Card detection callback from host.
 */
static void mmc_detect(struct mmc_host *host)
{
	int err;

	BUG_ON(!host);
	BUG_ON(!host->card);

	mmc_claim_host(host);

	/*
	 * Just check if our card has been removed.
	 */
	err = mmc_send_status(host->card, NULL);

	mmc_release_host(host);

	if (err) {
		mmc_remove(host);

		mmc_claim_host(host);
		mmc_detach_bus(host);
		mmc_release_host(host);
	}
}

/*
 * Suspend callback from host.
 */
static int mmc_suspend(struct mmc_host *host)
{
	BUG_ON(!host);
	BUG_ON(!host->card);

	mmc_claim_host(host);
	if (!mmc_host_is_spi(host))
		mmc_deselect_cards(host);
	host->card->state &= ~MMC_STATE_HIGHSPEED;
	mmc_release_host(host);

	return 0;
}

/*
 * Resume callback from host.
 *
 * This function tries to determine if the same card is still present
 * and, if so, restore all state to it.
 */
static int mmc_resume(struct mmc_host *host)
{
	int err;

	BUG_ON(!host);
	BUG_ON(!host->card);

	mmc_claim_host(host);
	err = mmc_init_card(host, host->ocr, host->card);
	mmc_release_host(host);

	return err;
}

static void mmc_power_restore(struct mmc_host *host)
{
	host->card->state &= ~MMC_STATE_HIGHSPEED;
	mmc_claim_host(host);
	mmc_init_card(host, host->ocr, host->card);
	mmc_release_host(host);
}

static int mmc_sleep(struct mmc_host *host)
{
	struct mmc_card *card = host->card;
	int err = -ENOSYS;

	if (card && card->ext_csd.rev >= 3) {
		err = mmc_card_sleepawake(host, 1);
		if (err < 0)
			pr_debug("%s: Error %d while putting card into sleep",
				 mmc_hostname(host), err);
	}

	return err;
}

static int mmc_awake(struct mmc_host *host)
{
	struct mmc_card *card = host->card;
	int err = -ENOSYS;

	if (card && card->ext_csd.rev >= 3) {
		err = mmc_card_sleepawake(host, 0);
		if (err < 0)
			pr_debug("%s: Error %d while awaking sleeping card",
				 mmc_hostname(host), err);
	}

	return err;
}

static const struct mmc_bus_ops mmc_ops = {
	.awake = mmc_awake,
	.sleep = mmc_sleep,
	.remove = mmc_remove,
	.detect = mmc_detect,
	.suspend = NULL,
	.resume = NULL,
	.power_restore = mmc_power_restore,
};

static const struct mmc_bus_ops mmc_ops_unsafe = {
	.awake = mmc_awake,
	.sleep = mmc_sleep,
	.remove = mmc_remove,
	.detect = mmc_detect,
	.suspend = mmc_suspend,
	.resume = mmc_resume,
	.power_restore = mmc_power_restore,
};

static void mmc_attach_bus_ops(struct mmc_host *host)
{
	const struct mmc_bus_ops *bus_ops;

	if (host->caps & MMC_CAP_NONREMOVABLE || !mmc_assume_removable)
		bus_ops = &mmc_ops_unsafe;
	else
		bus_ops = &mmc_ops;
	mmc_attach_bus(host, bus_ops);
}

/*
 * Starting point for MMC card init.
 */
int mmc_attach_mmc(struct mmc_host *host, u32 ocr)
{
	int err;

	BUG_ON(!host);
	WARN_ON(!host->claimed);

	mmc_attach_bus_ops(host);

	/*
	 * We need to get OCR a different way for SPI.
	 */
	if (mmc_host_is_spi(host)) {
		err = mmc_spi_read_ocr(host, 1, &ocr);
		if (err)
			goto err;
	}

	/*
	 * Sanity check the voltages that the card claims to
	 * support.
	 */
	if (ocr & 0x7F) {
		printk(KERN_WARNING "%s: card claims to support voltages "
		       "below the defined range. These will be ignored.\n",
		       mmc_hostname(host));
		ocr &= ~0x7F;
	}

	host->ocr = mmc_select_voltage(host, ocr);

	/*
	 * Can we support the voltage of the card?
	 */
	if (!host->ocr) {
		err = -EINVAL;
		goto err;
	}

	/*
	 * Detect and init the card.
	 */
	err = mmc_init_card(host, host->ocr, NULL);
	if (err)
		goto err;

	mmc_release_host(host);

	err = mmc_add_card(host->card);
	if (err)
		goto remove_card;

	return 0;

remove_card:
	mmc_remove_card(host->card);
	host->card = NULL;
	mmc_claim_host(host);
err:
	mmc_detach_bus(host);
	mmc_release_host(host);

	printk(KERN_ERR "%s: error %d whilst initialising MMC card\n",
		mmc_hostname(host), err);

	return err;
}
