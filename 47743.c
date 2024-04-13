static inline unsigned int round_pipe_size(unsigned int size)
{
	unsigned long nr_pages;

	nr_pages = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
	return roundup_pow_of_two(nr_pages) << PAGE_SHIFT;
}
