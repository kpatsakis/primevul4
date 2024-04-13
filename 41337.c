static void tg3_get_5906_nvram_info(struct tg3 *tp)
{
	tp->nvram_jedecnum = JEDEC_ATMEL;
	tg3_flag_set(tp, NVRAM_BUFFERED);
	tp->nvram_pagesize = ATMEL_AT24C512_CHIP_SIZE;
}
