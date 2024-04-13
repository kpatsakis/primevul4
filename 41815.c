xfs_buf_get_uncached(
	struct xfs_buftarg	*target,
	size_t			numblks,
	int			flags)
{
	unsigned long		page_count;
	int			error, i;
	struct xfs_buf		*bp;
	DEFINE_SINGLE_BUF_MAP(map, XFS_BUF_DADDR_NULL, numblks);

	bp = _xfs_buf_alloc(target, &map, 1, 0);
	if (unlikely(bp == NULL))
		goto fail;

	page_count = PAGE_ALIGN(numblks << BBSHIFT) >> PAGE_SHIFT;
	error = _xfs_buf_get_pages(bp, page_count, 0);
	if (error)
		goto fail_free_buf;

	for (i = 0; i < page_count; i++) {
		bp->b_pages[i] = alloc_page(xb_to_gfp(flags));
		if (!bp->b_pages[i])
			goto fail_free_mem;
	}
	bp->b_flags |= _XBF_PAGES;

	error = _xfs_buf_map_pages(bp, 0);
	if (unlikely(error)) {
		xfs_warn(target->bt_mount,
			"%s: failed to map pages\n", __func__);
		goto fail_free_mem;
	}

	trace_xfs_buf_get_uncached(bp, _RET_IP_);
	return bp;

 fail_free_mem:
	while (--i >= 0)
		__free_page(bp->b_pages[i]);
	_xfs_buf_free_pages(bp);
 fail_free_buf:
	xfs_buf_free_maps(bp);
	kmem_zone_free(xfs_buf_zone, bp);
 fail:
	return NULL;
}
