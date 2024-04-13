static void tg3_nvram_get_pagesize(struct tg3 *tp, u32 nvmcfg1)
{
	switch (nvmcfg1 & NVRAM_CFG1_5752PAGE_SIZE_MASK) {
	case FLASH_5752PAGE_SIZE_256:
		tp->nvram_pagesize = 256;
		break;
	case FLASH_5752PAGE_SIZE_512:
		tp->nvram_pagesize = 512;
		break;
	case FLASH_5752PAGE_SIZE_1K:
		tp->nvram_pagesize = 1024;
		break;
	case FLASH_5752PAGE_SIZE_2K:
		tp->nvram_pagesize = 2048;
		break;
	case FLASH_5752PAGE_SIZE_4K:
		tp->nvram_pagesize = 4096;
		break;
	case FLASH_5752PAGE_SIZE_264:
		tp->nvram_pagesize = 264;
		break;
	case FLASH_5752PAGE_SIZE_528:
		tp->nvram_pagesize = 528;
		break;
	}
}
