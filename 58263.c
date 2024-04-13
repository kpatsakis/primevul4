static const char *page_size_string(struct map_range *mr)
{
	static const char str_1g[] = "1G";
	static const char str_2m[] = "2M";
	static const char str_4m[] = "4M";
	static const char str_4k[] = "4k";

	if (mr->page_size_mask & (1<<PG_LEVEL_1G))
		return str_1g;
	/*
	 * 32-bit without PAE has a 4M large page size.
	 * PG_LEVEL_2M is misnamed, but we can at least
	 * print out the right size in the string.
	 */
	if (IS_ENABLED(CONFIG_X86_32) &&
	    !IS_ENABLED(CONFIG_X86_PAE) &&
	    mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_4m;

	if (mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_2m;

	return str_4k;
}
