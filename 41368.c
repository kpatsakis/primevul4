static int tg3_halt_cpu(struct tg3 *tp, u32 offset)
{
	int i;

	BUG_ON(offset == TX_CPU_BASE && tg3_flag(tp, 5705_PLUS));

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		u32 val = tr32(GRC_VCPU_EXT_CTRL);

		tw32(GRC_VCPU_EXT_CTRL, val | GRC_VCPU_EXT_CTRL_HALT_CPU);
		return 0;
	}
	if (offset == RX_CPU_BASE) {
		for (i = 0; i < 10000; i++) {
			tw32(offset + CPU_STATE, 0xffffffff);
			tw32(offset + CPU_MODE,  CPU_MODE_HALT);
			if (tr32(offset + CPU_MODE) & CPU_MODE_HALT)
				break;
		}

		tw32(offset + CPU_STATE, 0xffffffff);
		tw32_f(offset + CPU_MODE,  CPU_MODE_HALT);
		udelay(10);
	} else {
		/*
		 * There is only an Rx CPU for the 5750 derivative in the
		 * BCM4785.
		 */
		if (tg3_flag(tp, IS_SSB_CORE))
			return 0;

		for (i = 0; i < 10000; i++) {
			tw32(offset + CPU_STATE, 0xffffffff);
			tw32(offset + CPU_MODE,  CPU_MODE_HALT);
			if (tr32(offset + CPU_MODE) & CPU_MODE_HALT)
				break;
		}
	}

	if (i >= 10000) {
		netdev_err(tp->dev, "%s timed out, %s CPU\n",
			   __func__, offset == RX_CPU_BASE ? "RX" : "TX");
		return -ENODEV;
	}

	/* Clear firmware's nvram arbitration. */
	if (tg3_flag(tp, NVRAM))
		tw32(NVRAM_SWARB, SWARB_REQ_CLR0);
	return 0;
}
