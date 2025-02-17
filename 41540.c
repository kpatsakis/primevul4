static int tg3_test_registers(struct tg3 *tp)
{
	int i, is_5705, is_5750;
	u32 offset, read_mask, write_mask, val, save_val, read_val;
	static struct {
		u16 offset;
		u16 flags;
#define TG3_FL_5705	0x1
#define TG3_FL_NOT_5705	0x2
#define TG3_FL_NOT_5788	0x4
#define TG3_FL_NOT_5750	0x8
		u32 read_mask;
		u32 write_mask;
	} reg_tbl[] = {
		/* MAC Control Registers */
		{ MAC_MODE, TG3_FL_NOT_5705,
			0x00000000, 0x00ef6f8c },
		{ MAC_MODE, TG3_FL_5705,
			0x00000000, 0x01ef6b8c },
		{ MAC_STATUS, TG3_FL_NOT_5705,
			0x03800107, 0x00000000 },
		{ MAC_STATUS, TG3_FL_5705,
			0x03800100, 0x00000000 },
		{ MAC_ADDR_0_HIGH, 0x0000,
			0x00000000, 0x0000ffff },
		{ MAC_ADDR_0_LOW, 0x0000,
			0x00000000, 0xffffffff },
		{ MAC_RX_MTU_SIZE, 0x0000,
			0x00000000, 0x0000ffff },
		{ MAC_TX_MODE, 0x0000,
			0x00000000, 0x00000070 },
		{ MAC_TX_LENGTHS, 0x0000,
			0x00000000, 0x00003fff },
		{ MAC_RX_MODE, TG3_FL_NOT_5705,
			0x00000000, 0x000007fc },
		{ MAC_RX_MODE, TG3_FL_5705,
			0x00000000, 0x000007dc },
		{ MAC_HASH_REG_0, 0x0000,
			0x00000000, 0xffffffff },
		{ MAC_HASH_REG_1, 0x0000,
			0x00000000, 0xffffffff },
		{ MAC_HASH_REG_2, 0x0000,
			0x00000000, 0xffffffff },
		{ MAC_HASH_REG_3, 0x0000,
			0x00000000, 0xffffffff },

		/* Receive Data and Receive BD Initiator Control Registers. */
		{ RCVDBDI_JUMBO_BD+0, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ RCVDBDI_JUMBO_BD+4, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ RCVDBDI_JUMBO_BD+8, TG3_FL_NOT_5705,
			0x00000000, 0x00000003 },
		{ RCVDBDI_JUMBO_BD+0xc, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ RCVDBDI_STD_BD+0, 0x0000,
			0x00000000, 0xffffffff },
		{ RCVDBDI_STD_BD+4, 0x0000,
			0x00000000, 0xffffffff },
		{ RCVDBDI_STD_BD+8, 0x0000,
			0x00000000, 0xffff0002 },
		{ RCVDBDI_STD_BD+0xc, 0x0000,
			0x00000000, 0xffffffff },

		/* Receive BD Initiator Control Registers. */
		{ RCVBDI_STD_THRESH, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ RCVBDI_STD_THRESH, TG3_FL_5705,
			0x00000000, 0x000003ff },
		{ RCVBDI_JUMBO_THRESH, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },

		/* Host Coalescing Control Registers. */
		{ HOSTCC_MODE, TG3_FL_NOT_5705,
			0x00000000, 0x00000004 },
		{ HOSTCC_MODE, TG3_FL_5705,
			0x00000000, 0x000000f6 },
		{ HOSTCC_RXCOL_TICKS, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_RXCOL_TICKS, TG3_FL_5705,
			0x00000000, 0x000003ff },
		{ HOSTCC_TXCOL_TICKS, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_TXCOL_TICKS, TG3_FL_5705,
			0x00000000, 0x000003ff },
		{ HOSTCC_RXMAX_FRAMES, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_RXMAX_FRAMES, TG3_FL_5705 | TG3_FL_NOT_5788,
			0x00000000, 0x000000ff },
		{ HOSTCC_TXMAX_FRAMES, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_TXMAX_FRAMES, TG3_FL_5705 | TG3_FL_NOT_5788,
			0x00000000, 0x000000ff },
		{ HOSTCC_RXCOAL_TICK_INT, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_TXCOAL_TICK_INT, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_RXCOAL_MAXF_INT, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_RXCOAL_MAXF_INT, TG3_FL_5705 | TG3_FL_NOT_5788,
			0x00000000, 0x000000ff },
		{ HOSTCC_TXCOAL_MAXF_INT, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_TXCOAL_MAXF_INT, TG3_FL_5705 | TG3_FL_NOT_5788,
			0x00000000, 0x000000ff },
		{ HOSTCC_STAT_COAL_TICKS, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_STATS_BLK_HOST_ADDR, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_STATS_BLK_HOST_ADDR+4, TG3_FL_NOT_5705,
			0x00000000, 0xffffffff },
		{ HOSTCC_STATUS_BLK_HOST_ADDR, 0x0000,
			0x00000000, 0xffffffff },
		{ HOSTCC_STATUS_BLK_HOST_ADDR+4, 0x0000,
			0x00000000, 0xffffffff },
		{ HOSTCC_STATS_BLK_NIC_ADDR, 0x0000,
			0xffffffff, 0x00000000 },
		{ HOSTCC_STATUS_BLK_NIC_ADDR, 0x0000,
			0xffffffff, 0x00000000 },

		/* Buffer Manager Control Registers. */
		{ BUFMGR_MB_POOL_ADDR, TG3_FL_NOT_5750,
			0x00000000, 0x007fff80 },
		{ BUFMGR_MB_POOL_SIZE, TG3_FL_NOT_5750,
			0x00000000, 0x007fffff },
		{ BUFMGR_MB_RDMA_LOW_WATER, 0x0000,
			0x00000000, 0x0000003f },
		{ BUFMGR_MB_MACRX_LOW_WATER, 0x0000,
			0x00000000, 0x000001ff },
		{ BUFMGR_MB_HIGH_WATER, 0x0000,
			0x00000000, 0x000001ff },
		{ BUFMGR_DMA_DESC_POOL_ADDR, TG3_FL_NOT_5705,
			0xffffffff, 0x00000000 },
		{ BUFMGR_DMA_DESC_POOL_SIZE, TG3_FL_NOT_5705,
			0xffffffff, 0x00000000 },

		/* Mailbox Registers */
		{ GRCMBOX_RCVSTD_PROD_IDX+4, 0x0000,
			0x00000000, 0x000001ff },
		{ GRCMBOX_RCVJUMBO_PROD_IDX+4, TG3_FL_NOT_5705,
			0x00000000, 0x000001ff },
		{ GRCMBOX_RCVRET_CON_IDX_0+4, 0x0000,
			0x00000000, 0x000007ff },
		{ GRCMBOX_SNDHOST_PROD_IDX_0+4, 0x0000,
			0x00000000, 0x000001ff },

		{ 0xffff, 0x0000, 0x00000000, 0x00000000 },
	};

	is_5705 = is_5750 = 0;
	if (tg3_flag(tp, 5705_PLUS)) {
		is_5705 = 1;
		if (tg3_flag(tp, 5750_PLUS))
			is_5750 = 1;
	}

	for (i = 0; reg_tbl[i].offset != 0xffff; i++) {
		if (is_5705 && (reg_tbl[i].flags & TG3_FL_NOT_5705))
			continue;

		if (!is_5705 && (reg_tbl[i].flags & TG3_FL_5705))
			continue;

		if (tg3_flag(tp, IS_5788) &&
		    (reg_tbl[i].flags & TG3_FL_NOT_5788))
			continue;

		if (is_5750 && (reg_tbl[i].flags & TG3_FL_NOT_5750))
			continue;

		offset = (u32) reg_tbl[i].offset;
		read_mask = reg_tbl[i].read_mask;
		write_mask = reg_tbl[i].write_mask;

		/* Save the original register content */
		save_val = tr32(offset);

		/* Determine the read-only value. */
		read_val = save_val & read_mask;

		/* Write zero to the register, then make sure the read-only bits
		 * are not changed and the read/write bits are all zeros.
		 */
		tw32(offset, 0);

		val = tr32(offset);

		/* Test the read-only and read/write bits. */
		if (((val & read_mask) != read_val) || (val & write_mask))
			goto out;

		/* Write ones to all the bits defined by RdMask and WrMask, then
		 * make sure the read-only bits are not changed and the
		 * read/write bits are all ones.
		 */
		tw32(offset, read_mask | write_mask);

		val = tr32(offset);

		/* Test the read-only bits. */
		if ((val & read_mask) != read_val)
			goto out;

		/* Test the read/write bits. */
		if ((val & write_mask) != write_mask)
			goto out;

		tw32(offset, save_val);
	}

	return 0;

out:
	if (netif_msg_hw(tp))
		netdev_err(tp->dev,
			   "Register test failed at offset %x\n", offset);
	tw32(offset, save_val);
	return -EIO;
}
