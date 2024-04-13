xfs_seek_data(
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

	lock = xfs_ilock_data_map_shared(ip);

	isize = i_size_read(inode);
	if (start >= isize) {
		error = ENXIO;
		goto out_unlock;
	}

	/*
	 * Try to read extents from the first block indicated
	 * by fsbno to the end block of the file.
	 */
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

			/* Landed in a data extent */
			if (map[i].br_startblock == DELAYSTARTBLOCK ||
			    (map[i].br_state == XFS_EXT_NORM &&
			     !isnullstartblock(map[i].br_startblock)))
				goto out;

			/*
			 * Landed in an unwritten extent, try to search data
			 * from page cache.
			 */
			if (map[i].br_state == XFS_EXT_UNWRITTEN) {
				if (xfs_find_get_desired_pgoff(inode, &map[i],
							DATA_OFF, &offset))
					goto out;
			}
		}

		/*
		 * map[0] is hole or its an unwritten extent but
		 * without data in page cache.  Probably means that
		 * we are reading after EOF if nothing in map[1].
		 */
		if (nmap == 1) {
			error = ENXIO;
			goto out_unlock;
		}

		ASSERT(i > 1);

		/*
		 * Nothing was found, proceed to the next round of search
		 * if reading offset not beyond or hit EOF.
		 */
		fsbno = map[i - 1].br_startoff + map[i - 1].br_blockcount;
		start = XFS_FSB_TO_B(mp, fsbno);
		if (start >= isize) {
			error = ENXIO;
			goto out_unlock;
		}
	}

out:
	offset = vfs_setpos(file, offset, inode->i_sb->s_maxbytes);

out_unlock:
	xfs_iunlock(ip, lock);

	if (error)
		return -error;
	return offset;
}
