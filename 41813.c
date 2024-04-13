xfs_buf_get_map(
	struct xfs_buftarg	*target,
	struct xfs_buf_map	*map,
	int			nmaps,
	xfs_buf_flags_t		flags)
{
	struct xfs_buf		*bp;
	struct xfs_buf		*new_bp;
	int			error = 0;

	bp = _xfs_buf_find(target, map, nmaps, flags, NULL);
	if (likely(bp))
		goto found;

	new_bp = _xfs_buf_alloc(target, map, nmaps, flags);
	if (unlikely(!new_bp))
		return NULL;

	error = xfs_buf_allocate_memory(new_bp, flags);
	if (error) {
		xfs_buf_free(new_bp);
		return NULL;
	}

	bp = _xfs_buf_find(target, map, nmaps, flags, new_bp);
	if (!bp) {
		xfs_buf_free(new_bp);
		return NULL;
	}

	if (bp != new_bp)
		xfs_buf_free(new_bp);

found:
	if (!bp->b_addr) {
		error = _xfs_buf_map_pages(bp, flags);
		if (unlikely(error)) {
			xfs_warn(target->bt_mount,
				"%s: failed to map pages\n", __func__);
			xfs_buf_relse(bp);
			return NULL;
		}
	}

	XFS_STATS_INC(xb_get);
	trace_xfs_buf_get(bp, flags, _RET_IP_);
	return bp;
}
