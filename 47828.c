static unsigned long mmap_rnd(void)
{
	unsigned long rnd = 0;

	/*
	*  8 bits of randomness in 32bit mmaps, 20 address space bits
	* 28 bits of randomness in 64bit mmaps, 40 address space bits
	*/
	if (current->flags & PF_RANDOMIZE) {
		if (mmap_is_ia32())
			rnd = get_random_int() % (1<<8);
		else
			rnd = get_random_int() % (1<<28);
	}
	return rnd << PAGE_SHIFT;
}
