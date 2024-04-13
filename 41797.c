_xfs_buf_map_pages(
	xfs_buf_t		*bp,
	uint			flags)
{
	ASSERT(bp->b_flags & _XBF_PAGES);
	if (bp->b_page_count == 1) {
		/* A single page buffer is always mappable */
		bp->b_addr = page_address(bp->b_pages[0]) + bp->b_offset;
	} else if (flags & XBF_UNMAPPED) {
		bp->b_addr = NULL;
	} else {
		int retried = 0;

		do {
			bp->b_addr = vm_map_ram(bp->b_pages, bp->b_page_count,
						-1, PAGE_KERNEL);
			if (bp->b_addr)
				break;
			vm_unmap_aliases();
		} while (retried++ <= 1);

		if (!bp->b_addr)
			return -ENOMEM;
		bp->b_addr += bp->b_offset;
	}

	return 0;
}
