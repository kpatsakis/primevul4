static void tg3_nvram_init(struct tg3 *tp)
{
	if (tg3_flag(tp, IS_SSB_CORE)) {
		/* No NVRAM and EEPROM on the SSB Broadcom GigE core. */
		tg3_flag_clear(tp, NVRAM);
		tg3_flag_clear(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, NO_NVRAM);
		return;
	}

	tw32_f(GRC_EEPROM_ADDR,
	     (EEPROM_ADDR_FSM_RESET |
	      (EEPROM_DEFAULT_CLOCK_PERIOD <<
	       EEPROM_ADDR_CLKPERD_SHIFT)));

	msleep(1);

	/* Enable seeprom accesses. */
	tw32_f(GRC_LOCAL_CTRL,
	     tr32(GRC_LOCAL_CTRL) | GRC_LCLCTRL_AUTO_SEEPROM);
	udelay(100);

	if (tg3_asic_rev(tp) != ASIC_REV_5700 &&
	    tg3_asic_rev(tp) != ASIC_REV_5701) {
		tg3_flag_set(tp, NVRAM);

		if (tg3_nvram_lock(tp)) {
			netdev_warn(tp->dev,
				    "Cannot get nvram lock, %s failed\n",
				    __func__);
			return;
		}
		tg3_enable_nvram_access(tp);

		tp->nvram_size = 0;

		if (tg3_asic_rev(tp) == ASIC_REV_5752)
			tg3_get_5752_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5755)
			tg3_get_5755_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5787 ||
			 tg3_asic_rev(tp) == ASIC_REV_5784 ||
			 tg3_asic_rev(tp) == ASIC_REV_5785)
			tg3_get_5787_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5761)
			tg3_get_5761_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5906)
			tg3_get_5906_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_57780 ||
			 tg3_flag(tp, 57765_CLASS))
			tg3_get_57780_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
			 tg3_asic_rev(tp) == ASIC_REV_5719)
			tg3_get_5717_nvram_info(tp);
		else if (tg3_asic_rev(tp) == ASIC_REV_5720 ||
			 tg3_asic_rev(tp) == ASIC_REV_5762)
			tg3_get_5720_nvram_info(tp);
		else
			tg3_get_nvram_info(tp);

		if (tp->nvram_size == 0)
			tg3_get_nvram_size(tp);

		tg3_disable_nvram_access(tp);
		tg3_nvram_unlock(tp);

	} else {
		tg3_flag_clear(tp, NVRAM);
		tg3_flag_clear(tp, NVRAM_BUFFERED);

		tg3_get_eeprom_size(tp);
	}
}
