_xfs_buf_ioend(
	xfs_buf_t		*bp,
	int			schedule)
{
	if (atomic_dec_and_test(&bp->b_io_remaining) == 1)
		xfs_buf_ioend(bp, schedule);
}
