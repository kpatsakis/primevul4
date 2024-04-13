xfs_da3_swap_lastblock(
	struct xfs_da_args	*args,
	xfs_dablk_t		*dead_blknop,
	struct xfs_buf		**dead_bufp)
{
	struct xfs_da_blkinfo	*dead_info;
	struct xfs_da_blkinfo	*sib_info;
	struct xfs_da_intnode	*par_node;
	struct xfs_da_intnode	*dead_node;
	struct xfs_dir2_leaf	*dead_leaf2;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr par_hdr;
	struct xfs_inode	*dp;
	struct xfs_trans	*tp;
	struct xfs_mount	*mp;
	struct xfs_buf		*dead_buf;
	struct xfs_buf		*last_buf;
	struct xfs_buf		*sib_buf;
	struct xfs_buf		*par_buf;
	xfs_dahash_t		dead_hash;
	xfs_fileoff_t		lastoff;
	xfs_dablk_t		dead_blkno;
	xfs_dablk_t		last_blkno;
	xfs_dablk_t		sib_blkno;
	xfs_dablk_t		par_blkno;
	int			error;
	int			w;
	int			entno;
	int			level;
	int			dead_level;

	trace_xfs_da_swap_lastblock(args);

	dead_buf = *dead_bufp;
	dead_blkno = *dead_blknop;
	tp = args->trans;
	dp = args->dp;
	w = args->whichfork;
	ASSERT(w == XFS_DATA_FORK);
	mp = dp->i_mount;
	lastoff = mp->m_dirfreeblk;
	error = xfs_bmap_last_before(tp, dp, &lastoff, w);
	if (error)
		return error;
	if (unlikely(lastoff == 0)) {
		XFS_ERROR_REPORT("xfs_da_swap_lastblock(1)", XFS_ERRLEVEL_LOW,
				 mp);
		return XFS_ERROR(EFSCORRUPTED);
	}
	/*
	 * Read the last block in the btree space.
	 */
	last_blkno = (xfs_dablk_t)lastoff - mp->m_dirblkfsbs;
	error = xfs_da3_node_read(tp, dp, last_blkno, -1, &last_buf, w);
	if (error)
		return error;
	/*
	 * Copy the last block into the dead buffer and log it.
	 */
	memcpy(dead_buf->b_addr, last_buf->b_addr, mp->m_dirblksize);
	xfs_trans_log_buf(tp, dead_buf, 0, mp->m_dirblksize - 1);
	dead_info = dead_buf->b_addr;
	/*
	 * Get values from the moved block.
	 */
	if (dead_info->magic == cpu_to_be16(XFS_DIR2_LEAFN_MAGIC) ||
	    dead_info->magic == cpu_to_be16(XFS_DIR3_LEAFN_MAGIC)) {
		struct xfs_dir3_icleaf_hdr leafhdr;
		struct xfs_dir2_leaf_entry *ents;

		dead_leaf2 = (xfs_dir2_leaf_t *)dead_info;
		dp->d_ops->leaf_hdr_from_disk(&leafhdr, dead_leaf2);
		ents = dp->d_ops->leaf_ents_p(dead_leaf2);
		dead_level = 0;
		dead_hash = be32_to_cpu(ents[leafhdr.count - 1].hashval);
	} else {
		struct xfs_da3_icnode_hdr deadhdr;

		dead_node = (xfs_da_intnode_t *)dead_info;
		dp->d_ops->node_hdr_from_disk(&deadhdr, dead_node);
		btree = dp->d_ops->node_tree_p(dead_node);
		dead_level = deadhdr.level;
		dead_hash = be32_to_cpu(btree[deadhdr.count - 1].hashval);
	}
	sib_buf = par_buf = NULL;
	/*
	 * If the moved block has a left sibling, fix up the pointers.
	 */
	if ((sib_blkno = be32_to_cpu(dead_info->back))) {
		error = xfs_da3_node_read(tp, dp, sib_blkno, -1, &sib_buf, w);
		if (error)
			goto done;
		sib_info = sib_buf->b_addr;
		if (unlikely(
		    be32_to_cpu(sib_info->forw) != last_blkno ||
		    sib_info->magic != dead_info->magic)) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(2)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		sib_info->forw = cpu_to_be32(dead_blkno);
		xfs_trans_log_buf(tp, sib_buf,
			XFS_DA_LOGRANGE(sib_info, &sib_info->forw,
					sizeof(sib_info->forw)));
		sib_buf = NULL;
	}
	/*
	 * If the moved block has a right sibling, fix up the pointers.
	 */
	if ((sib_blkno = be32_to_cpu(dead_info->forw))) {
		error = xfs_da3_node_read(tp, dp, sib_blkno, -1, &sib_buf, w);
		if (error)
			goto done;
		sib_info = sib_buf->b_addr;
		if (unlikely(
		       be32_to_cpu(sib_info->back) != last_blkno ||
		       sib_info->magic != dead_info->magic)) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(3)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		sib_info->back = cpu_to_be32(dead_blkno);
		xfs_trans_log_buf(tp, sib_buf,
			XFS_DA_LOGRANGE(sib_info, &sib_info->back,
					sizeof(sib_info->back)));
		sib_buf = NULL;
	}
	par_blkno = mp->m_dirleafblk;
	level = -1;
	/*
	 * Walk down the tree looking for the parent of the moved block.
	 */
	for (;;) {
		error = xfs_da3_node_read(tp, dp, par_blkno, -1, &par_buf, w);
		if (error)
			goto done;
		par_node = par_buf->b_addr;
		dp->d_ops->node_hdr_from_disk(&par_hdr, par_node);
		if (level >= 0 && level != par_hdr.level + 1) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(4)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		level = par_hdr.level;
		btree = dp->d_ops->node_tree_p(par_node);
		for (entno = 0;
		     entno < par_hdr.count &&
		     be32_to_cpu(btree[entno].hashval) < dead_hash;
		     entno++)
			continue;
		if (entno == par_hdr.count) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(5)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		par_blkno = be32_to_cpu(btree[entno].before);
		if (level == dead_level + 1)
			break;
		xfs_trans_brelse(tp, par_buf);
		par_buf = NULL;
	}
	/*
	 * We're in the right parent block.
	 * Look for the right entry.
	 */
	for (;;) {
		for (;
		     entno < par_hdr.count &&
		     be32_to_cpu(btree[entno].before) != last_blkno;
		     entno++)
			continue;
		if (entno < par_hdr.count)
			break;
		par_blkno = par_hdr.forw;
		xfs_trans_brelse(tp, par_buf);
		par_buf = NULL;
		if (unlikely(par_blkno == 0)) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(6)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		error = xfs_da3_node_read(tp, dp, par_blkno, -1, &par_buf, w);
		if (error)
			goto done;
		par_node = par_buf->b_addr;
		dp->d_ops->node_hdr_from_disk(&par_hdr, par_node);
		if (par_hdr.level != level) {
			XFS_ERROR_REPORT("xfs_da_swap_lastblock(7)",
					 XFS_ERRLEVEL_LOW, mp);
			error = XFS_ERROR(EFSCORRUPTED);
			goto done;
		}
		btree = dp->d_ops->node_tree_p(par_node);
		entno = 0;
	}
	/*
	 * Update the parent entry pointing to the moved block.
	 */
	btree[entno].before = cpu_to_be32(dead_blkno);
	xfs_trans_log_buf(tp, par_buf,
		XFS_DA_LOGRANGE(par_node, &btree[entno].before,
				sizeof(btree[entno].before)));
	*dead_blknop = last_blkno;
	*dead_bufp = last_buf;
	return 0;
done:
	if (par_buf)
		xfs_trans_brelse(tp, par_buf);
	if (sib_buf)
		xfs_trans_brelse(tp, sib_buf);
	xfs_trans_brelse(tp, last_buf);
	return error;
}
