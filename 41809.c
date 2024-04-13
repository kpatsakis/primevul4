xfs_buf_delwri_submit(
	struct list_head	*buffer_list)
{
	LIST_HEAD		(io_list);
	int			error = 0, error2;
	struct xfs_buf		*bp;

	__xfs_buf_delwri_submit(buffer_list, &io_list, true);

	/* Wait for IO to complete. */
	while (!list_empty(&io_list)) {
		bp = list_first_entry(&io_list, struct xfs_buf, b_list);

		list_del_init(&bp->b_list);
		error2 = xfs_buf_iowait(bp);
		xfs_buf_relse(bp);
		if (!error)
			error = error2;
	}

	return error;
}
