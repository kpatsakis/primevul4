static u32 tg3_nvram_phys_addr(struct tg3 *tp, u32 addr)
{
	if (tg3_flag(tp, NVRAM) &&
	    tg3_flag(tp, NVRAM_BUFFERED) &&
	    tg3_flag(tp, FLASH) &&
	    !tg3_flag(tp, NO_NVRAM_ADDR_TRANS) &&
	    (tp->nvram_jedecnum == JEDEC_ATMEL))

		addr = ((addr / tp->nvram_pagesize) <<
			ATMEL_AT45DB0X1B_PAGE_POS) +
		       (addr % tp->nvram_pagesize);

	return addr;
}
