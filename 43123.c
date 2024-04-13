xfs_dabuf_map(
	struct xfs_trans	*trans,
	struct xfs_inode	*dp,
	xfs_dablk_t		bno,
	xfs_daddr_t		mappedbno,
	int			whichfork,
	struct xfs_buf_map	**map,
	int			*nmaps)
{
	struct xfs_mount	*mp = dp->i_mount;
	int			nfsb;
	int			error = 0;
	struct xfs_bmbt_irec	irec;
	struct xfs_bmbt_irec	*irecs = &irec;
	int			nirecs;

	ASSERT(map && *map);
	ASSERT(*nmaps == 1);

	nfsb = (whichfork == XFS_DATA_FORK) ? mp->m_dirblkfsbs : 1;

	/*
	 * Caller doesn't have a mapping.  -2 means don't complain
	 * if we land in a hole.
	 */
	if (mappedbno == -1 || mappedbno == -2) {
		/*
		 * Optimize the one-block case.
		 */
		if (nfsb != 1)
			irecs = kmem_zalloc(sizeof(irec) * nfsb,
					    KM_SLEEP | KM_NOFS);

		nirecs = nfsb;
		error = xfs_bmapi_read(dp, (xfs_fileoff_t)bno, nfsb, irecs,
				       &nirecs, xfs_bmapi_aflag(whichfork));
		if (error)
			goto out;
	} else {
		irecs->br_startblock = XFS_DADDR_TO_FSB(mp, mappedbno);
		irecs->br_startoff = (xfs_fileoff_t)bno;
		irecs->br_blockcount = nfsb;
		irecs->br_state = 0;
		nirecs = 1;
	}

	if (!xfs_da_map_covers_blocks(nirecs, irecs, bno, nfsb)) {
		error = mappedbno == -2 ? -1 : XFS_ERROR(EFSCORRUPTED);
		if (unlikely(error == EFSCORRUPTED)) {
			if (xfs_error_level >= XFS_ERRLEVEL_LOW) {
				int i;
				xfs_alert(mp, "%s: bno %lld dir: inode %lld",
					__func__, (long long)bno,
					(long long)dp->i_ino);
				for (i = 0; i < *nmaps; i++) {
					xfs_alert(mp,
"[%02d] br_startoff %lld br_startblock %lld br_blockcount %lld br_state %d",
						i,
						(long long)irecs[i].br_startoff,
						(long long)irecs[i].br_startblock,
						(long long)irecs[i].br_blockcount,
						irecs[i].br_state);
				}
			}
			XFS_ERROR_REPORT("xfs_da_do_buf(1)",
					 XFS_ERRLEVEL_LOW, mp);
		}
		goto out;
	}
	error = xfs_buf_map_from_irec(mp, map, nmaps, irecs, nirecs);
out:
	if (irecs != &irec)
		kmem_free(irecs);
	return error;
}
