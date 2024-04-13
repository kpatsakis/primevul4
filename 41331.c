static void tg3_get_5720_nvram_info(struct tg3 *tp)
{
	u32 nvcfg1, nvmpinstrp;

	nvcfg1 = tr32(NVRAM_CFG1);
	nvmpinstrp = nvcfg1 & NVRAM_CFG1_5752VENDOR_MASK;

	if (tg3_asic_rev(tp) == ASIC_REV_5762) {
		if (!(nvcfg1 & NVRAM_CFG1_5762VENDOR_MASK)) {
			tg3_flag_set(tp, NO_NVRAM);
			return;
		}

		switch (nvmpinstrp) {
		case FLASH_5762_EEPROM_HD:
			nvmpinstrp = FLASH_5720_EEPROM_HD;
			break;
		case FLASH_5762_EEPROM_LD:
			nvmpinstrp = FLASH_5720_EEPROM_LD;
			break;
		}
	}

	switch (nvmpinstrp) {
	case FLASH_5720_EEPROM_HD:
	case FLASH_5720_EEPROM_LD:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);

		nvcfg1 &= ~NVRAM_CFG1_COMPAT_BYPASS;
		tw32(NVRAM_CFG1, nvcfg1);
		if (nvmpinstrp == FLASH_5720_EEPROM_HD)
			tp->nvram_pagesize = ATMEL_AT24C512_CHIP_SIZE;
		else
			tp->nvram_pagesize = ATMEL_AT24C02_CHIP_SIZE;
		return;
	case FLASH_5720VENDOR_M_ATMEL_DB011D:
	case FLASH_5720VENDOR_A_ATMEL_DB011B:
	case FLASH_5720VENDOR_A_ATMEL_DB011D:
	case FLASH_5720VENDOR_M_ATMEL_DB021D:
	case FLASH_5720VENDOR_A_ATMEL_DB021B:
	case FLASH_5720VENDOR_A_ATMEL_DB021D:
	case FLASH_5720VENDOR_M_ATMEL_DB041D:
	case FLASH_5720VENDOR_A_ATMEL_DB041B:
	case FLASH_5720VENDOR_A_ATMEL_DB041D:
	case FLASH_5720VENDOR_M_ATMEL_DB081D:
	case FLASH_5720VENDOR_A_ATMEL_DB081D:
	case FLASH_5720VENDOR_ATMEL_45USPT:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvmpinstrp) {
		case FLASH_5720VENDOR_M_ATMEL_DB021D:
		case FLASH_5720VENDOR_A_ATMEL_DB021B:
		case FLASH_5720VENDOR_A_ATMEL_DB021D:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		case FLASH_5720VENDOR_M_ATMEL_DB041D:
		case FLASH_5720VENDOR_A_ATMEL_DB041B:
		case FLASH_5720VENDOR_A_ATMEL_DB041D:
			tp->nvram_size = TG3_NVRAM_SIZE_512KB;
			break;
		case FLASH_5720VENDOR_M_ATMEL_DB081D:
		case FLASH_5720VENDOR_A_ATMEL_DB081D:
			tp->nvram_size = TG3_NVRAM_SIZE_1MB;
			break;
		default:
			if (tg3_asic_rev(tp) != ASIC_REV_5762)
				tp->nvram_size = TG3_NVRAM_SIZE_128KB;
			break;
		}
		break;
	case FLASH_5720VENDOR_M_ST_M25PE10:
	case FLASH_5720VENDOR_M_ST_M45PE10:
	case FLASH_5720VENDOR_A_ST_M25PE10:
	case FLASH_5720VENDOR_A_ST_M45PE10:
	case FLASH_5720VENDOR_M_ST_M25PE20:
	case FLASH_5720VENDOR_M_ST_M45PE20:
	case FLASH_5720VENDOR_A_ST_M25PE20:
	case FLASH_5720VENDOR_A_ST_M45PE20:
	case FLASH_5720VENDOR_M_ST_M25PE40:
	case FLASH_5720VENDOR_M_ST_M45PE40:
	case FLASH_5720VENDOR_A_ST_M25PE40:
	case FLASH_5720VENDOR_A_ST_M45PE40:
	case FLASH_5720VENDOR_M_ST_M25PE80:
	case FLASH_5720VENDOR_M_ST_M45PE80:
	case FLASH_5720VENDOR_A_ST_M25PE80:
	case FLASH_5720VENDOR_A_ST_M45PE80:
	case FLASH_5720VENDOR_ST_25USPT:
	case FLASH_5720VENDOR_ST_45USPT:
		tp->nvram_jedecnum = JEDEC_ST;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);

		switch (nvmpinstrp) {
		case FLASH_5720VENDOR_M_ST_M25PE20:
		case FLASH_5720VENDOR_M_ST_M45PE20:
		case FLASH_5720VENDOR_A_ST_M25PE20:
		case FLASH_5720VENDOR_A_ST_M45PE20:
			tp->nvram_size = TG3_NVRAM_SIZE_256KB;
			break;
		case FLASH_5720VENDOR_M_ST_M25PE40:
		case FLASH_5720VENDOR_M_ST_M45PE40:
		case FLASH_5720VENDOR_A_ST_M25PE40:
		case FLASH_5720VENDOR_A_ST_M45PE40:
			tp->nvram_size = TG3_NVRAM_SIZE_512KB;
			break;
		case FLASH_5720VENDOR_M_ST_M25PE80:
		case FLASH_5720VENDOR_M_ST_M45PE80:
		case FLASH_5720VENDOR_A_ST_M25PE80:
		case FLASH_5720VENDOR_A_ST_M45PE80:
			tp->nvram_size = TG3_NVRAM_SIZE_1MB;
			break;
		default:
			if (tg3_asic_rev(tp) != ASIC_REV_5762)
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

	if (tg3_asic_rev(tp) == ASIC_REV_5762) {
		u32 val;

		if (tg3_nvram_read(tp, 0, &val))
			return;

		if (val != TG3_EEPROM_MAGIC &&
		    (val & TG3_EEPROM_MAGIC_FW_MSK) != TG3_EEPROM_MAGIC_FW)
			tg3_flag_set(tp, NO_NVRAM);
	}
}
