static void __init memory_map_bottom_up(unsigned long map_start,
					unsigned long map_end)
{
	unsigned long next, start;
	unsigned long mapped_ram_size = 0;
	/* step_size need to be small so pgt_buf from BRK could cover it */
	unsigned long step_size = PMD_SIZE;

	start = map_start;
	min_pfn_mapped = start >> PAGE_SHIFT;

	/*
	 * We start from the bottom (@map_start) and go to the top (@map_end).
	 * The memblock_find_in_range() gets us a block of RAM from the
	 * end of RAM in [min_pfn_mapped, max_pfn_mapped) used as new pages
	 * for page table.
	 */
	while (start < map_end) {
		if (step_size && map_end - start > step_size) {
			next = round_up(start + 1, step_size);
			if (next > map_end)
				next = map_end;
		} else {
			next = map_end;
		}

		mapped_ram_size += init_range_memory_mapping(start, next);
		start = next;

		if (mapped_ram_size >= step_size)
			step_size = get_new_step_size(step_size);
	}
}
