__xfs_buf_delwri_submit(
	struct list_head	*buffer_list,
	struct list_head	*io_list,
	bool			wait)
{
	struct blk_plug		plug;
	struct xfs_buf		*bp, *n;
	int			pinned = 0;

	list_for_each_entry_safe(bp, n, buffer_list, b_list) {
		if (!wait) {
			if (xfs_buf_ispinned(bp)) {
				pinned++;
				continue;
			}
			if (!xfs_buf_trylock(bp))
				continue;
		} else {
			xfs_buf_lock(bp);
		}

		/*
		 * Someone else might have written the buffer synchronously or
		 * marked it stale in the meantime.  In that case only the
		 * _XBF_DELWRI_Q flag got cleared, and we have to drop the
		 * reference and remove it from the list here.
		 */
		if (!(bp->b_flags & _XBF_DELWRI_Q)) {
			list_del_init(&bp->b_list);
			xfs_buf_relse(bp);
			continue;
		}

		list_move_tail(&bp->b_list, io_list);
		trace_xfs_buf_delwri_split(bp, _RET_IP_);
	}

	list_sort(NULL, io_list, xfs_buf_cmp);

	blk_start_plug(&plug);
	list_for_each_entry_safe(bp, n, io_list, b_list) {
		bp->b_flags &= ~(_XBF_DELWRI_Q | XBF_ASYNC);
		bp->b_flags |= XBF_WRITE;

		if (!wait) {
			bp->b_flags |= XBF_ASYNC;
			list_del_init(&bp->b_list);
		}
		xfs_bdstrat_cb(bp);
	}
	blk_finish_plug(&plug);

	return pinned;
}
