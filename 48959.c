xfs_seek_hole(
	struct file		*file,
	loff_t			start)
{
	struct inode		*inode = file->f_mapping->host;
	struct xfs_inode	*ip = XFS_I(inode);
	struct xfs_mount	*mp = ip->i_mount;
	loff_t			uninitialized_var(offset);
	xfs_fsize_t		isize;
	xfs_fileoff_t		fsbno;
	xfs_filblks_t		end;
	uint			lock;
	int			error;

	if (XFS_FORCED_SHUTDOWN(mp))
		return -XFS_ERROR(EIO);

	lock = xfs_ilock_data_map_shared(ip);

	isize = i_size_read(inode);
	if (start >= isize) {
		error = ENXIO;
		goto out_unlock;
	}

	fsbno = XFS_B_TO_FSBT(mp, start);
	end = XFS_B_TO_FSB(mp, isize);

	for (;;) {
		struct xfs_bmbt_irec	map[2];
		int			nmap = 2;
		unsigned int		i;

		error = xfs_bmapi_read(ip, fsbno, end - fsbno, map, &nmap,
				       XFS_BMAPI_ENTIRE);
		if (error)
			goto out_unlock;

		/* No extents at given offset, must be beyond EOF */
		if (nmap == 0) {
			error = ENXIO;
			goto out_unlock;
		}

		for (i = 0; i < nmap; i++) {
			offset = max_t(loff_t, start,
				       XFS_FSB_TO_B(mp, map[i].br_startoff));

			/* Landed in a hole */
			if (map[i].br_startblock == HOLESTARTBLOCK)
				goto out;

			/*
			 * Landed in an unwritten extent, try to search hole
			 * from page cache.
			 */
			if (map[i].br_state == XFS_EXT_UNWRITTEN) {
				if (xfs_find_get_desired_pgoff(inode, &map[i],
							HOLE_OFF, &offset))
					goto out;
			}
		}

		/*
		 * map[0] contains data or its unwritten but contains
		 * data in page cache, probably means that we are
		 * reading after EOF.  We should fix offset to point
		 * to the end of the file(i.e., there is an implicit
		 * hole at the end of any file).
		 */
		if (nmap == 1) {
			offset = isize;
			break;
		}

		ASSERT(i > 1);

		/*
		 * Both mappings contains data, proceed to the next round of
		 * search if the current reading offset not beyond or hit EOF.
		 */
		fsbno = map[i - 1].br_startoff + map[i - 1].br_blockcount;
		start = XFS_FSB_TO_B(mp, fsbno);
		if (start >= isize) {
			offset = isize;
			break;
		}
	}

out:
	/*
	 * At this point, we must have found a hole.  However, the returned
	 * offset may be bigger than the file size as it may be aligned to
	 * page boundary for unwritten extents, we need to deal with this
	 * situation in particular.
	 */
	offset = min_t(loff_t, offset, isize);
	offset = vfs_setpos(file, offset, inode->i_sb->s_maxbytes);

out_unlock:
	xfs_iunlock(ip, lock);

	if (error)
		return -error;
	return offset;
}
