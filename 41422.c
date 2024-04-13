static int tg3_nvram_write_block(struct tg3 *tp, u32 offset, u32 len, u8 *buf)
{
	int ret;

	if (tg3_flag(tp, EEPROM_WRITE_PROT)) {
		tw32_f(GRC_LOCAL_CTRL, tp->grc_local_ctrl &
		       ~GRC_LCLCTRL_GPIO_OUTPUT1);
		udelay(40);
	}

	if (!tg3_flag(tp, NVRAM)) {
		ret = tg3_nvram_write_block_using_eeprom(tp, offset, len, buf);
	} else {
		u32 grc_mode;

		ret = tg3_nvram_lock(tp);
		if (ret)
			return ret;

		tg3_enable_nvram_access(tp);
		if (tg3_flag(tp, 5750_PLUS) && !tg3_flag(tp, PROTECTED_NVRAM))
			tw32(NVRAM_WRITE1, 0x406);

		grc_mode = tr32(GRC_MODE);
		tw32(GRC_MODE, grc_mode | GRC_MODE_NVRAM_WR_ENABLE);

		if (tg3_flag(tp, NVRAM_BUFFERED) || !tg3_flag(tp, FLASH)) {
			ret = tg3_nvram_write_block_buffered(tp, offset, len,
				buf);
		} else {
			ret = tg3_nvram_write_block_unbuffered(tp, offset, len,
				buf);
		}

		grc_mode = tr32(GRC_MODE);
		tw32(GRC_MODE, grc_mode & ~GRC_MODE_NVRAM_WR_ENABLE);

		tg3_disable_nvram_access(tp);
		tg3_nvram_unlock(tp);
	}

	if (tg3_flag(tp, EEPROM_WRITE_PROT)) {
		tw32_f(GRC_LOCAL_CTRL, tp->grc_local_ctrl);
		udelay(40);
	}

	return ret;
}
