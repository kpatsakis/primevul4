xfs_lookup_buffer_offset(
	struct page		*page,
	loff_t			*offset,
	unsigned int		type)
{
	loff_t			lastoff = page_offset(page);
	bool			found = false;
	struct buffer_head	*bh, *head;

	bh = head = page_buffers(page);
	do {
		/*
		 * Unwritten extents that have data in the page
		 * cache covering them can be identified by the
		 * BH_Unwritten state flag.  Pages with multiple
		 * buffers might have a mix of holes, data and
		 * unwritten extents - any buffer with valid
		 * data in it should have BH_Uptodate flag set
		 * on it.
		 */
		if (buffer_unwritten(bh) ||
		    buffer_uptodate(bh)) {
			if (type == DATA_OFF)
				found = true;
		} else {
			if (type == HOLE_OFF)
				found = true;
		}

		if (found) {
			*offset = lastoff;
			break;
		}
		lastoff += bh->b_size;
	} while ((bh = bh->b_this_page) != head);

	return found;
}
