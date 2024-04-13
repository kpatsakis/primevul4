xfs_da_map_covers_blocks(
	int		nmap,
	xfs_bmbt_irec_t	*mapp,
	xfs_dablk_t	bno,
	int		count)
{
	int		i;
	xfs_fileoff_t	off;

	for (i = 0, off = bno; i < nmap; i++) {
		if (mapp[i].br_startblock == HOLESTARTBLOCK ||
		    mapp[i].br_startblock == DELAYSTARTBLOCK) {
			return 0;
		}
		if (off != mapp[i].br_startoff) {
			return 0;
		}
		off += mapp[i].br_blockcount;
	}
	return off == bno + count;
}
