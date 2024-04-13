xfs_da3_root_split(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*blk1,
	struct xfs_da_state_blk	*blk2)
{
	struct xfs_da_intnode	*node;
	struct xfs_da_intnode	*oldroot;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr nodehdr;
	struct xfs_da_args	*args;
	struct xfs_buf		*bp;
	struct xfs_inode	*dp;
	struct xfs_trans	*tp;
	struct xfs_mount	*mp;
	struct xfs_dir2_leaf	*leaf;
	xfs_dablk_t		blkno;
	int			level;
	int			error;
	int			size;

	trace_xfs_da_root_split(state->args);

	/*
	 * Copy the existing (incorrect) block from the root node position
	 * to a free space somewhere.
	 */
	args = state->args;
	error = xfs_da_grow_inode(args, &blkno);
	if (error)
		return error;

	dp = args->dp;
	tp = args->trans;
	mp = state->mp;
	error = xfs_da_get_buf(tp, dp, blkno, -1, &bp, args->whichfork);
	if (error)
		return error;
	node = bp->b_addr;
	oldroot = blk1->bp->b_addr;
	if (oldroot->hdr.info.magic == cpu_to_be16(XFS_DA_NODE_MAGIC) ||
	    oldroot->hdr.info.magic == cpu_to_be16(XFS_DA3_NODE_MAGIC)) {
		struct xfs_da3_icnode_hdr nodehdr;

		dp->d_ops->node_hdr_from_disk(&nodehdr, oldroot);
		btree = dp->d_ops->node_tree_p(oldroot);
		size = (int)((char *)&btree[nodehdr.count] - (char *)oldroot);
		level = nodehdr.level;

		/*
		 * we are about to copy oldroot to bp, so set up the type
		 * of bp while we know exactly what it will be.
		 */
		xfs_trans_buf_set_type(tp, bp, XFS_BLFT_DA_NODE_BUF);
	} else {
		struct xfs_dir3_icleaf_hdr leafhdr;
		struct xfs_dir2_leaf_entry *ents;

		leaf = (xfs_dir2_leaf_t *)oldroot;
		dp->d_ops->leaf_hdr_from_disk(&leafhdr, leaf);
		ents = dp->d_ops->leaf_ents_p(leaf);

		ASSERT(leafhdr.magic == XFS_DIR2_LEAFN_MAGIC ||
		       leafhdr.magic == XFS_DIR3_LEAFN_MAGIC);
		size = (int)((char *)&ents[leafhdr.count] - (char *)leaf);
		level = 0;

		/*
		 * we are about to copy oldroot to bp, so set up the type
		 * of bp while we know exactly what it will be.
		 */
		xfs_trans_buf_set_type(tp, bp, XFS_BLFT_DIR_LEAFN_BUF);
	}

	/*
	 * we can copy most of the information in the node from one block to
	 * another, but for CRC enabled headers we have to make sure that the
	 * block specific identifiers are kept intact. We update the buffer
	 * directly for this.
	 */
	memcpy(node, oldroot, size);
	if (oldroot->hdr.info.magic == cpu_to_be16(XFS_DA3_NODE_MAGIC) ||
	    oldroot->hdr.info.magic == cpu_to_be16(XFS_DIR3_LEAFN_MAGIC)) {
		struct xfs_da3_intnode *node3 = (struct xfs_da3_intnode *)node;

		node3->hdr.info.blkno = cpu_to_be64(bp->b_bn);
	}
	xfs_trans_log_buf(tp, bp, 0, size - 1);

	bp->b_ops = blk1->bp->b_ops;
	xfs_trans_buf_copy_type(bp, blk1->bp);
	blk1->bp = bp;
	blk1->blkno = blkno;

	/*
	 * Set up the new root node.
	 */
	error = xfs_da3_node_create(args,
		(args->whichfork == XFS_DATA_FORK) ? mp->m_dirleafblk : 0,
		level + 1, &bp, args->whichfork);
	if (error)
		return error;

	node = bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);
	btree = dp->d_ops->node_tree_p(node);
	btree[0].hashval = cpu_to_be32(blk1->hashval);
	btree[0].before = cpu_to_be32(blk1->blkno);
	btree[1].hashval = cpu_to_be32(blk2->hashval);
	btree[1].before = cpu_to_be32(blk2->blkno);
	nodehdr.count = 2;
	dp->d_ops->node_hdr_to_disk(node, &nodehdr);

#ifdef DEBUG
	if (oldroot->hdr.info.magic == cpu_to_be16(XFS_DIR2_LEAFN_MAGIC) ||
	    oldroot->hdr.info.magic == cpu_to_be16(XFS_DIR3_LEAFN_MAGIC)) {
		ASSERT(blk1->blkno >= mp->m_dirleafblk &&
		       blk1->blkno < mp->m_dirfreeblk);
		ASSERT(blk2->blkno >= mp->m_dirleafblk &&
		       blk2->blkno < mp->m_dirfreeblk);
	}
#endif

	/* Header is already logged by xfs_da_node_create */
	xfs_trans_log_buf(tp, bp,
		XFS_DA_LOGRANGE(node, btree, sizeof(xfs_da_node_entry_t) * 2));

	return 0;
}
