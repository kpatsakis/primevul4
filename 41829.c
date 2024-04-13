xfs_buf_offset(
	xfs_buf_t		*bp,
	size_t			offset)
{
	struct page		*page;

	if (bp->b_addr)
		return bp->b_addr + offset;

	offset += bp->b_offset;
	page = bp->b_pages[offset >> PAGE_SHIFT];
	return (xfs_caddr_t)page_address(page) + (offset & (PAGE_SIZE-1));
}
