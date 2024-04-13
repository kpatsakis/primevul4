xfs_buf_vmap_len(
	struct xfs_buf	*bp)
{
	return (bp->b_page_count * PAGE_SIZE) - bp->b_offset;
}
