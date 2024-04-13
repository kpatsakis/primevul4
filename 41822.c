xfs_buf_iomove(
	xfs_buf_t		*bp,	/* buffer to process		*/
	size_t			boff,	/* starting buffer offset	*/
	size_t			bsize,	/* length to copy		*/
	void			*data,	/* data address			*/
	xfs_buf_rw_t		mode)	/* read/write/zero flag		*/
{
	size_t			bend;

	bend = boff + bsize;
	while (boff < bend) {
		struct page	*page;
		int		page_index, page_offset, csize;

		page_index = (boff + bp->b_offset) >> PAGE_SHIFT;
		page_offset = (boff + bp->b_offset) & ~PAGE_MASK;
		page = bp->b_pages[page_index];
		csize = min_t(size_t, PAGE_SIZE - page_offset,
				      BBTOB(bp->b_io_length) - boff);

		ASSERT((csize + page_offset) <= PAGE_SIZE);

		switch (mode) {
		case XBRW_ZERO:
			memset(page_address(page) + page_offset, 0, csize);
			break;
		case XBRW_READ:
			memcpy(data, page_address(page) + page_offset, csize);
			break;
		case XBRW_WRITE:
			memcpy(page_address(page) + page_offset, data, csize);
		}

		boff += csize;
		data += csize;
	}
}
