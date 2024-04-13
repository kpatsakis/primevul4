xfs_buf_ioerror_alert(
	struct xfs_buf		*bp,
	const char		*func)
{
	xfs_alert(bp->b_target->bt_mount,
"metadata I/O error: block 0x%llx (\"%s\") error %d numblks %d",
		(__uint64_t)XFS_BUF_ADDR(bp), func, bp->b_error, bp->b_length);
}
