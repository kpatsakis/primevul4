xfs_dir_open(
	struct inode	*inode,
	struct file	*file)
{
	struct xfs_inode *ip = XFS_I(inode);
	int		mode;
	int		error;

	error = xfs_file_open(inode, file);
	if (error)
		return error;

	/*
	 * If there are any blocks, read-ahead block 0 as we're almost
	 * certain to have the next operation be a read there.
	 */
	mode = xfs_ilock_data_map_shared(ip);
	if (ip->i_d.di_nextents > 0)
		xfs_dir3_data_readahead(NULL, ip, 0, -1);
	xfs_iunlock(ip, mode);
	return 0;
}
