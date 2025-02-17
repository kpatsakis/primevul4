static void tg3_get_57780_nvram_info(struct tg3 *tp)
{
	u32 nvcfg1;

	nvcfg1 = tr32(NVRAM_CFG1);

	switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
	case FLASH_5787VENDOR_ATMEL_EEPROM_376KHZ:
	case FLASH_5787VENDOR_MICRO_EEPROM_376KHZ:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tp->nvram_pagesize = ATMEL_AT24C512_CHIP_SIZE;

		nvcfg1 &= ~NVRAM_CFG1_COMPAT_BYPASS;
		tw32(NVRAM_CFG1, nvcfg1);
		return;
	case FLASH_5752VENDOR_ATMEL_FLASH_BUFFERED:
	case FLASH_57780VENDOR_ATMEL_AT45DB011D:
	case FLASH_57780VENDOR_ATMEL_AT45DB011B:
	case FLASH_57780VENDOR_ATMEL_AT45DB021D:
	case FLASH_57780VENDOR_ATMEL_AT45DB021B:
	case FLASH_57780VENDOR_ATMEL_AT45DB041D:
	case FLASH_57780VENDOR_ATMEL_AT45DB041B:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
		case FLASH_5752VENDOR_ATMEL_FLASH_BUFFERED:
		case FLASH_57780VENDOR_ATMEL_AT45DB011D:
		case FLASH_57780VENDOR_ATMEL_AT45DB011B:
			tp->nvram_size = TG3_NVRAM_SIZE_128KB;
			break;
		case FLASH_57780VENDOR_ATMEL_AT45DB021D:
		case FLASH_57780VENDOR_ATMEL_AT45DB021B:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		case FLASH_57780VENDOR_ATMEL_AT45DB041D:
		case FLASH_57780VENDOR_ATMEL_AT45DB041B:
			tp->nvram_size = TG3_NVRAM_SIZE_512KB;
			break;
		}
		break;
	case FLASH_5752VENDOR_ST_M45PE10:
	case FLASH_5752VENDOR_ST_M45PE20:
	case FLASH_5752VENDOR_ST_M45PE40:
		tp->nvram_jedecnum = JEDEC_ST;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK) {
		case FLASH_5752VENDOR_ST_M45PE10:
			tp->nvram_size = TG3_NVRAM_SIZE_128KB;
			break;
		case FLASH_5752VENDOR_ST_M45PE20:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		case FLASH_5752VENDOR_ST_M45PE40:
			tp->nvram_size = TG3_NVRAM_SIZE_512KB;
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
