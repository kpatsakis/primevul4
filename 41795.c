_xfs_buf_ioapply(
	struct xfs_buf	*bp)
{
	struct blk_plug	plug;
	int		rw;
	int		offset;
	int		size;
	int		i;

	if (bp->b_flags & XBF_WRITE) {
		if (bp->b_flags & XBF_SYNCIO)
			rw = WRITE_SYNC;
		else
			rw = WRITE;
		if (bp->b_flags & XBF_FUA)
			rw |= REQ_FUA;
		if (bp->b_flags & XBF_FLUSH)
			rw |= REQ_FLUSH;

		/*
		 * Run the write verifier callback function if it exists. If
		 * this function fails it will mark the buffer with an error and
		 * the IO should not be dispatched.
		 */
		if (bp->b_ops) {
			bp->b_ops->verify_write(bp);
			if (bp->b_error) {
				xfs_force_shutdown(bp->b_target->bt_mount,
						   SHUTDOWN_CORRUPT_INCORE);
				return;
			}
		}
	} else if (bp->b_flags & XBF_READ_AHEAD) {
		rw = READA;
	} else {
		rw = READ;
	}

	/* we only use the buffer cache for meta-data */
	rw |= REQ_META;

	/*
	 * Walk all the vectors issuing IO on them. Set up the initial offset
	 * into the buffer and the desired IO size before we start -
	 * _xfs_buf_ioapply_vec() will modify them appropriately for each
	 * subsequent call.
	 */
	offset = bp->b_offset;
	size = BBTOB(bp->b_io_length);
	blk_start_plug(&plug);
	for (i = 0; i < bp->b_map_count; i++) {
		xfs_buf_ioapply_map(bp, i, &offset, &size, rw);
		if (bp->b_error)
			break;
		if (size <= 0)
			break;	/* all done */
	}
	blk_finish_plug(&plug);
}
