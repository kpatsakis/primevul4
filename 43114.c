xfs_da_grow_inode(
	struct xfs_da_args	*args,
	xfs_dablk_t		*new_blkno)
{
	xfs_fileoff_t		bno;
	int			count;
	int			error;

	trace_xfs_da_grow_inode(args);

	if (args->whichfork == XFS_DATA_FORK) {
		bno = args->dp->i_mount->m_dirleafblk;
		count = args->dp->i_mount->m_dirblkfsbs;
	} else {
		bno = 0;
		count = 1;
	}

	error = xfs_da_grow_inode_int(args, &bno, count);
	if (!error)
		*new_blkno = (xfs_dablk_t)bno;
	return error;
}
