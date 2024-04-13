static inline unsigned long brk_rnd(void)
{
        unsigned long rnd = 0;

	/* 8MB for 32bit, 1GB for 64bit */
	if (is_32bit_task())
		rnd = (long)(get_random_int() % (1<<(23-PAGE_SHIFT)));
	else
		rnd = (long)(get_random_int() % (1<<(30-PAGE_SHIFT)));

	return rnd << PAGE_SHIFT;
}
