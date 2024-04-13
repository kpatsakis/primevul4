static void tg3_get_nvram_size(struct tg3 *tp)
{
	u32 val;

	if (tg3_flag(tp, NO_NVRAM) || tg3_nvram_read(tp, 0, &val) != 0)
		return;

	/* Selfboot format */
	if (val != TG3_EEPROM_MAGIC) {
		tg3_get_eeprom_size(tp);
		return;
	}

	if (tg3_nvram_read(tp, 0xf0, &val) == 0) {
		if (val != 0) {
			/* This is confusing.  We want to operate on the
			 * 16-bit value at offset 0xf2.  The tg3_nvram_read()
			 * call will read from NVRAM and byteswap the data
			 * according to the byteswapping settings for all
			 * other register accesses.  This ensures the data we
			 * want will always reside in the lower 16-bits.
			 * However, the data in NVRAM is in LE format, which
			 * means the data from the NVRAM read will always be
			 * opposite the endianness of the CPU.  The 16-bit
			 * byteswap then brings the data to CPU endianness.
			 */
			tp->nvram_size = swab16((u16)(val & 0x0000ffff)) * 1024;
			return;
		}
	}
	tp->nvram_size = TG3_NVRAM_SIZE_512KB;
}
