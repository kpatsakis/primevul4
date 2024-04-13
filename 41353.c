static void tg3_get_nvram_info(struct tg3 *tp)
{
	u32 nvcfg1;

	nvcfg1 = tr32(NVRAM_CFG1);
	if (nvcfg1 & NVRAM_CFG1_FLASHIF_ENAB) {
		tg3_flag_set(tp, FLASH);
	} else {
		nvcfg1 &= ~NVRAM_CFG1_COMPAT_BYPASS;
		tw32(NVRAM_CFG1, nvcfg1);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5750 ||
	    tg3_flag(tp, 5780_CLASS)) {
		switch (nvcfg1 & NVRAM_CFG1_VENDOR_MASK) {
		case FLASH_VENDOR_ATMEL_FLASH_BUFFERED:
			tp->nvram_jedecnum = JEDEC_ATMEL;
			tp->nvram_pagesize = ATMEL_AT45DB0X1B_PAGE_SIZE;
			tg3_flag_set(tp, NVRAM_BUFFERED);
			break;
		case FLASH_VENDOR_ATMEL_FLASH_UNBUFFERED:
			tp->nvram_jedecnum = JEDEC_ATMEL;
			tp->nvram_pagesize = ATMEL_AT25F512_PAGE_SIZE;
			break;
		case FLASH_VENDOR_ATMEL_EEPROM:
			tp->nvram_jedecnum = JEDEC_ATMEL;
			tp->nvram_pagesize = ATMEL_AT24C512_CHIP_SIZE;
			tg3_flag_set(tp, NVRAM_BUFFERED);
			break;
		case FLASH_VENDOR_ST:
			tp->nvram_jedecnum = JEDEC_ST;
			tp->nvram_pagesize = ST_M45PEX0_PAGE_SIZE;
			tg3_flag_set(tp, NVRAM_BUFFERED);
			break;
		case FLASH_VENDOR_SAIFUN:
			tp->nvram_jedecnum = JEDEC_SAIFUN;
			tp->nvram_pagesize = SAIFUN_SA25F0XX_PAGE_SIZE;
			break;
		case FLASH_VENDOR_SST_SMALL:
		case FLASH_VENDOR_SST_LARGE:
			tp->nvram_jedecnum = JEDEC_SST;
			tp->nvram_pagesize = SST_25VF0X0_PAGE_SIZE;
			break;
		}
	} else {
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tp->nvram_pagesize = ATMEL_AT45DB0X1B_PAGE_SIZE;
		tg3_flag_set(tp, NVRAM_BUFFERED);
	}
}
