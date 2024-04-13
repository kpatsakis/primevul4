static int tg3_poll_fw(struct tg3 *tp)
{
	int i;
	u32 val;

	if (tg3_flag(tp, IS_SSB_CORE)) {
		/* We don't use firmware. */
		return 0;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		/* Wait up to 20ms for init done. */
		for (i = 0; i < 200; i++) {
			if (tr32(VCPU_STATUS) & VCPU_STATUS_INIT_DONE)
				return 0;
			udelay(100);
		}
		return -ENODEV;
	}

	/* Wait for firmware initialization to complete. */
	for (i = 0; i < 100000; i++) {
		tg3_read_mem(tp, NIC_SRAM_FIRMWARE_MBOX, &val);
		if (val == ~NIC_SRAM_FIRMWARE_MBOX_MAGIC1)
			break;
		udelay(10);
	}

	/* Chip might not be fitted with firmware.  Some Sun onboard
	 * parts are configured like that.  So don't signal the timeout
	 * of the above loop as an error, but do report the lack of
	 * running firmware once.
	 */
	if (i >= 100000 && !tg3_flag(tp, NO_FWARE_REPORTED)) {
		tg3_flag_set(tp, NO_FWARE_REPORTED);

		netdev_info(tp->dev, "No firmware running\n");
	}

	if (tg3_chip_rev_id(tp) == CHIPREV_ID_57765_A0) {
		/* The 57765 A0 needs a little more
		 * time to do some important work.
		 */
		mdelay(10);
	}

	return 0;
}
