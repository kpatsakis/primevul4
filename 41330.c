static void tg3_get_5717_nvram_info(struct tg3 *tp)
{
	u32 nvcfg1;

	nvcfg1 = tr32(NVRAM_CFG1);

	switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
	case FLASH_5717VENDOR_ATMEL_EEPROM:
	case FLASH_5717VENDOR_MICRO_EEPROM:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tp->nvram_pagesize = ATMEL_AT24C512_CHIP_SIZE;

		nvcfg1 &= ~NVRAM_CFG1_COMPAT_BYPASS;
		tw32(NVRAM_CFG1, nvcfg1);
		return;
	case FLASH_5717VENDOR_ATMEL_MDB011D:
	case FLASH_5717VENDOR_ATMEL_ADB011B:
	case FLASH_5717VENDOR_ATMEL_ADB011D:
	case FLASH_5717VENDOR_ATMEL_MDB021D:
	case FLASH_5717VENDOR_ATMEL_ADB021B:
	case FLASH_5717VENDOR_ATMEL_ADB021D:
	case FLASH_5717VENDOR_ATMEL_45USPT:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
		case FLASH_5717VENDOR_ATMEL_MDB021D:
			/* Detect size with tg3_nvram_get_size() */
			break;
		case FLASH_5717VENDOR_ATMEL_ADB021B:
		case FLASH_5717VENDOR_ATMEL_ADB021D:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		default:
			tp->nvram_size = TG3_NVRAM_SIZE_128KB;
			break;
		}
		break;
	case FLASH_5717VENDOR_ST_M_M25PE10:
	case FLASH_5717VENDOR_ST_A_M25PE10:
	case FLASH_5717VENDOR_ST_M_M45PE10:
	case FLASH_5717VENDOR_ST_A_M45PE10:
	case FLASH_5717VENDOR_ST_M_M25PE20:
	case FLASH_5717VENDOR_ST_A_M25PE20:
	case FLASH_5717VENDOR_ST_M_M45PE20:
	case FLASH_5717VENDOR_ST_A_M45PE20:
	case FLASH_5717VENDOR_ST_25USPT:
	case FLASH_5717VENDOR_ST_45USPT:
		tp->nvram_jedecnum = JEDEC_ST;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
		case FLASH_5717VENDOR_ST_M_M25PE20:
		case FLASH_5717VENDOR_ST_M_M45PE20:
			/* Detect size with tg3_nvram_get_size() */
			break;
		case FLASH_5717VENDOR_ST_A_M25PE20:
		case FLASH_5717VENDOR_ST_A_M45PE20:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		default:
			tp->nvram_size = TG3_NVRAM_SIZE_128KB;
			break;
		}
		break;
	default:
		tg3_flag_set(tp, NO_NVRAM);
		return;
	}

	tg3_nvram_get_pagesize(tp, nvcfg1);
	if (tp->nvram_pagesize != 264 && tp->nvram_pagesize != 528)
		tg3_flag_set(tp, NO_NVRAM_ADDR_TRANS);
}
