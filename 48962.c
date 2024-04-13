xfs_zero_last_block(
	struct xfs_inode	*ip,
	xfs_fsize_t		offset,
	xfs_fsize_t		isize)
{
	struct xfs_mount	*mp = ip->i_mount;
	xfs_fileoff_t		last_fsb = XFS_B_TO_FSBT(mp, isize);
	int			zero_offset = XFS_B_FSB_OFFSET(mp, isize);
	int			zero_len;
	int			nimaps = 1;
	int			error = 0;
	struct xfs_bmbt_irec	imap;

	xfs_ilock(ip, XFS_ILOCK_EXCL);
	error = xfs_bmapi_read(ip, last_fsb, 1, &imap, &nimaps, 0);
	xfs_iunlock(ip, XFS_ILOCK_EXCL);
	if (error)
		return error;

	ASSERT(nimaps > 0);

	/*
	 * If the block underlying isize is just a hole, then there
	 * is nothing to zero.
	 */
	if (imap.br_startblock == HOLESTARTBLOCK)
		return 0;

	zero_len = mp->m_sb.sb_blocksize - zero_offset;
	if (isize + zero_len > offset)
		zero_len = offset - isize;
	return xfs_iozero(ip, isize, zero_len);
}
