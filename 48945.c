xfs_file_fallocate(
	struct file		*file,
	int			mode,
	loff_t			offset,
	loff_t			len)
{
	struct inode		*inode = file_inode(file);
	struct xfs_inode	*ip = XFS_I(inode);
	struct xfs_trans	*tp;
	long			error;
	loff_t			new_size = 0;

	if (!S_ISREG(inode->i_mode))
		return -EINVAL;
	if (mode & ~(FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE |
		     FALLOC_FL_COLLAPSE_RANGE | FALLOC_FL_ZERO_RANGE))
		return -EOPNOTSUPP;

	xfs_ilock(ip, XFS_IOLOCK_EXCL);
	if (mode & FALLOC_FL_PUNCH_HOLE) {
		error = xfs_free_file_space(ip, offset, len);
		if (error)
			goto out_unlock;
	} else if (mode & FALLOC_FL_COLLAPSE_RANGE) {
		unsigned blksize_mask = (1 << inode->i_blkbits) - 1;

		if (offset & blksize_mask || len & blksize_mask) {
			error = -EINVAL;
			goto out_unlock;
		}

		/*
		 * There is no need to overlap collapse range with EOF,
		 * in which case it is effectively a truncate operation
		 */
		if (offset + len >= i_size_read(inode)) {
			error = -EINVAL;
			goto out_unlock;
		}

		new_size = i_size_read(inode) - len;

		error = xfs_collapse_file_space(ip, offset, len);
		if (error)
			goto out_unlock;
	} else {
		if (!(mode & FALLOC_FL_KEEP_SIZE) &&
		    offset + len > i_size_read(inode)) {
			new_size = offset + len;
			error = -inode_newsize_ok(inode, new_size);
			if (error)
				goto out_unlock;
		}

		if (mode & FALLOC_FL_ZERO_RANGE)
			error = xfs_zero_file_space(ip, offset, len);
		else
			error = xfs_alloc_file_space(ip, offset, len,
						     XFS_BMAPI_PREALLOC);
		if (error)
			goto out_unlock;
	}

	tp = xfs_trans_alloc(ip->i_mount, XFS_TRANS_WRITEID);
	error = xfs_trans_reserve(tp, &M_RES(ip->i_mount)->tr_writeid, 0, 0);
	if (error) {
		xfs_trans_cancel(tp, 0);
		goto out_unlock;
	}

	xfs_ilock(ip, XFS_ILOCK_EXCL);
	xfs_trans_ijoin(tp, ip, XFS_ILOCK_EXCL);
	ip->i_d.di_mode &= ~S_ISUID;
	if (ip->i_d.di_mode & S_IXGRP)
		ip->i_d.di_mode &= ~S_ISGID;

	if (!(mode & (FALLOC_FL_PUNCH_HOLE | FALLOC_FL_COLLAPSE_RANGE)))
		ip->i_d.di_flags |= XFS_DIFLAG_PREALLOC;

	xfs_trans_ichgtime(tp, ip, XFS_ICHGTIME_MOD | XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	if (file->f_flags & O_DSYNC)
		xfs_trans_set_sync(tp);
	error = xfs_trans_commit(tp, 0);
	if (error)
		goto out_unlock;

	/* Change file size if needed */
	if (new_size) {
		struct iattr iattr;

		iattr.ia_valid = ATTR_SIZE;
		iattr.ia_size = new_size;
		error = xfs_setattr_size(ip, &iattr);
	}

out_unlock:
	xfs_iunlock(ip, XFS_IOLOCK_EXCL);
	return -error;
}
