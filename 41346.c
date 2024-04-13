static void tg3_get_eeprom_size(struct tg3 *tp)
{
	u32 cursize, val, magic;

	tp->nvram_size = EEPROM_CHIP_SIZE;

	if (tg3_nvram_read(tp, 0, &magic) != 0)
		return;

	if ((magic != TG3_EEPROM_MAGIC) &&
	    ((magic & TG3_EEPROM_MAGIC_FW_MSK) != TG3_EEPROM_MAGIC_FW) &&
	    ((magic & TG3_EEPROM_MAGIC_HW_MSK) != TG3_EEPROM_MAGIC_HW))
		return;

	/*
	 * Size the chip by reading offsets at increasing powers of two.
	 * When we encounter our validation signature, we know the addressing
	 * has wrapped around, and thus have our chip size.
	 */
	cursize = 0x10;

	while (cursize < tp->nvram_size) {
		if (tg3_nvram_read(tp, cursize, &val) != 0)
			return;

		if (val == magic)
			break;

		cursize <<= 1;
	}

	tp->nvram_size = cursize;
}
