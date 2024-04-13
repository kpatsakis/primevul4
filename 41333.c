static void tg3_get_5755_nvram_info(struct tg3 *tp)
{
	u32 nvcfg1, protect = 0;

	nvcfg1 = tr32(NVRAM_CFG1);

	/* NVRAM protection for TPM */
	if (nvcfg1 & (1 << 27)) {
		tg3_flag_set(tp, PROTECTED_NVRAM);
		protect = 1;
	}

	nvcfg1 &= NVRAM_CFG1_5752VENDOR_MASK;
	switch (nvcfg1) {
	case FLASH_5755VENDOR_ATMEL_FLASH_1:
	case FLASH_5755VENDOR_ATMEL_FLASH_2:
	case FLASH_5755VENDOR_ATMEL_FLASH_3:
	case FLASH_5755VENDOR_ATMEL_FLASH_5:
		tp->nvram_jedecnum = JEDEC_ATMEL;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);
		tp->nvram_pagesize = 264;
		if (nvcfg1 == FLASH_5755VENDOR_ATMEL_FLASH_1 ||
		    nvcfg1 == FLASH_5755VENDOR_ATMEL_FLASH_5)
			tp->nvram_size = (protect ? 0x3e200 :
					  TG3_NVRAM_SIZE_512KB);
		else if (nvcfg1 == FLASH_5755VENDOR_ATMEL_FLASH_2)
			tp->nvram_size = (protect ? 0x1f200 :
					  TG3_NVRAM_SIZE_256KB);
		else
			tp->nvram_size = (protect ? 0x1f200 :
					  TG3_NVRAM_SIZE_128KB);
		break;
	case FLASH_5752VENDOR_ST_M45PE10:
	case FLASH_5752VENDOR_ST_M45PE20:
	case FLASH_5752VENDOR_ST_M45PE40:
		tp->nvram_jedecnum = JEDEC_ST;
		tg3_flag_set(tp, NVRAM_BUFFERED);
		tg3_flag_set(tp, FLASH);
		tp->nvram_pagesize = 256;
		if (nvcfg1 == FLASH_5752VENDOR_ST_M45PE10)
			tp->nvram_size = (protect ?
					  TG3_NVRAM_SIZE_64KB :
					  TG3_NVRAM_SIZE_128KB);
		else if (nvcfg1 == FLASH_5752VENDOR_ST_M45PE20)
			tp->nvram_size = (protect ?
					  TG3_NVRAM_SIZE_64KB :
					  TG3_NVRAM_SIZE_256KB);
		else
			tp->nvram_size = (protect ?
					  TG3_NVRAM_SIZE_128KB :
					  TG3_NVRAM_SIZE_512KB);
		break;
	}
}
