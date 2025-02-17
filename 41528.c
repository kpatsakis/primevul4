static int tg3_stop_block(struct tg3 *tp, unsigned long ofs, u32 enable_bit, int silent)
{
	unsigned int i;
	u32 val;

	if (tg3_flag(tp, 5705_PLUS)) {
		switch (ofs) {
		case RCVLSC_MODE:
		case DMAC_MODE:
		case MBFREE_MODE:
		case BUFMGR_MODE:
		case MEMARB_MODE:
			/* We can't enable/disable these bits of the
			 * 5705/5750, just say success.
			 */
			return 0;

		default:
			break;
		}
	}

	val = tr32(ofs);
	val &= ~enable_bit;
	tw32_f(ofs, val);

	for (i = 0; i < MAX_WAIT_CNT; i++) {
		udelay(100);
		val = tr32(ofs);
		if ((val & enable_bit) == 0)
			break;
	}

	if (i == MAX_WAIT_CNT && !silent) {
		dev_err(&tp->pdev->dev,
			"tg3_stop_block timed out, ofs=%lx enable_bit=%x\n",
			ofs, enable_bit);
		return -ENODEV;
	}

	return 0;
}
