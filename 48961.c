xfs_zero_eof(
	struct xfs_inode	*ip,
	xfs_off_t		offset,		/* starting I/O offset */
	xfs_fsize_t		isize)		/* current inode size */
{
	struct xfs_mount	*mp = ip->i_mount;
	xfs_fileoff_t		start_zero_fsb;
	xfs_fileoff_t		end_zero_fsb;
	xfs_fileoff_t		zero_count_fsb;
	xfs_fileoff_t		last_fsb;
	xfs_fileoff_t		zero_off;
	xfs_fsize_t		zero_len;
	int			nimaps;
	int			error = 0;
	struct xfs_bmbt_irec	imap;

	ASSERT(xfs_isilocked(ip, XFS_IOLOCK_EXCL));
	ASSERT(offset > isize);

	/*
	 * First handle zeroing the block on which isize resides.
	 *
	 * We only zero a part of that block so it is handled specially.
	 */
	if (XFS_B_FSB_OFFSET(mp, isize) != 0) {
		error = xfs_zero_last_block(ip, offset, isize);
		if (error)
			return error;
	}

	/*
	 * Calculate the range between the new size and the old where blocks
	 * needing to be zeroed may exist.
	 *
	 * To get the block where the last byte in the file currently resides,
	 * we need to subtract one from the size and truncate back to a block
	 * boundary.  We subtract 1 in case the size is exactly on a block
	 * boundary.
	 */
	last_fsb = isize ? XFS_B_TO_FSBT(mp, isize - 1) : (xfs_fileoff_t)-1;
	start_zero_fsb = XFS_B_TO_FSB(mp, (xfs_ufsize_t)isize);
	end_zero_fsb = XFS_B_TO_FSBT(mp, offset - 1);
	ASSERT((xfs_sfiloff_t)last_fsb < (xfs_sfiloff_t)start_zero_fsb);
	if (last_fsb == end_zero_fsb) {
		/*
		 * The size was only incremented on its last block.
		 * We took care of that above, so just return.
		 */
		return 0;
	}

	ASSERT(start_zero_fsb <= end_zero_fsb);
	while (start_zero_fsb <= end_zero_fsb) {
		nimaps = 1;
		zero_count_fsb = end_zero_fsb - start_zero_fsb + 1;

		xfs_ilock(ip, XFS_ILOCK_EXCL);
		error = xfs_bmapi_read(ip, start_zero_fsb, zero_count_fsb,
					  &imap, &nimaps, 0);
		xfs_iunlock(ip, XFS_ILOCK_EXCL);
		if (error)
			return error;

		ASSERT(nimaps > 0);

		if (imap.br_state == XFS_EXT_UNWRITTEN ||
		    imap.br_startblock == HOLESTARTBLOCK) {
			start_zero_fsb = imap.br_startoff + imap.br_blockcount;
			ASSERT(start_zero_fsb <= (end_zero_fsb + 1));
			continue;
		}

		/*
		 * There are blocks we need to zero.
		 */
		zero_off = XFS_FSB_TO_B(mp, start_zero_fsb);
		zero_len = XFS_FSB_TO_B(mp, imap.br_blockcount);

		if ((zero_off + zero_len) > offset)
			zero_len = offset - zero_off;

		error = xfs_iozero(ip, zero_off, zero_len);
		if (error)
			return error;

		start_zero_fsb = imap.br_startoff + imap.br_blockcount;
		ASSERT(start_zero_fsb <= (end_zero_fsb + 1));
	}

	return 0;
}
