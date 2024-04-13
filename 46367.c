mm_zalloc(struct mm_master *mm, u_int ncount, u_int size)
{
	size_t len = (size_t) size * ncount;
	void *address;

	if (len == 0 || ncount > SIZE_MAX / size)
		fatal("%s: mm_zalloc(%u, %u)", __func__, ncount, size);

	address = mm_malloc(mm, len);

	return (address);
}
