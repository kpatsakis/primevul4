xfs_buf_set_empty(
	struct xfs_buf		*bp,
	size_t			numblks)
{
	if (bp->b_pages)
		_xfs_buf_free_pages(bp);

	bp->b_pages = NULL;
	bp->b_page_count = 0;
	bp->b_addr = NULL;
	bp->b_length = numblks;
	bp->b_io_length = numblks;

	ASSERT(bp->b_map_count == 1);
	bp->b_bn = XFS_BUF_DADDR_NULL;
	bp->b_maps[0].bm_bn = XFS_BUF_DADDR_NULL;
	bp->b_maps[0].bm_len = bp->b_length;
}
