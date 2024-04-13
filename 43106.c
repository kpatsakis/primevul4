xfs_da3_path_shift(
	struct xfs_da_state	*state,
	struct xfs_da_state_path *path,
	int			forward,
	int			release,
	int			*result)
{
	struct xfs_da_state_blk	*blk;
	struct xfs_da_blkinfo	*info;
	struct xfs_da_intnode	*node;
	struct xfs_da_args	*args;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr nodehdr;
	xfs_dablk_t		blkno = 0;
	int			level;
	int			error;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_path_shift(state->args);

	/*
	 * Roll up the Btree looking for the first block where our
	 * current index is not at the edge of the block.  Note that
	 * we skip the bottom layer because we want the sibling block.
	 */
	args = state->args;
	ASSERT(args != NULL);
	ASSERT(path != NULL);
	ASSERT((path->active > 0) && (path->active < XFS_DA_NODE_MAXDEPTH));
	level = (path->active-1) - 1;	/* skip bottom layer in path */
	for (blk = &path->blk[level]; level >= 0; blk--, level--) {
		node = blk->bp->b_addr;
		dp->d_ops->node_hdr_from_disk(&nodehdr, node);
		btree = dp->d_ops->node_tree_p(node);

		if (forward && (blk->index < nodehdr.count - 1)) {
			blk->index++;
			blkno = be32_to_cpu(btree[blk->index].before);
			break;
		} else if (!forward && (blk->index > 0)) {
			blk->index--;
			blkno = be32_to_cpu(btree[blk->index].before);
			break;
		}
	}
	if (level < 0) {
		*result = XFS_ERROR(ENOENT);	/* we're out of our tree */
		ASSERT(args->op_flags & XFS_DA_OP_OKNOENT);
		return(0);
	}

	/*
	 * Roll down the edge of the subtree until we reach the
	 * same depth we were at originally.
	 */
	for (blk++, level++; level < path->active; blk++, level++) {
		/*
		 * Release the old block.
		 * (if it's dirty, trans won't actually let go)
		 */
		if (release)
			xfs_trans_brelse(args->trans, blk->bp);

		/*
		 * Read the next child block.
		 */
		blk->blkno = blkno;
		error = xfs_da3_node_read(args->trans, dp, blkno, -1,
					&blk->bp, args->whichfork);
		if (error)
			return(error);
		info = blk->bp->b_addr;
		ASSERT(info->magic == cpu_to_be16(XFS_DA_NODE_MAGIC) ||
		       info->magic == cpu_to_be16(XFS_DA3_NODE_MAGIC) ||
		       info->magic == cpu_to_be16(XFS_DIR2_LEAFN_MAGIC) ||
		       info->magic == cpu_to_be16(XFS_DIR3_LEAFN_MAGIC) ||
		       info->magic == cpu_to_be16(XFS_ATTR_LEAF_MAGIC) ||
		       info->magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC));


		/*
		 * Note: we flatten the magic number to a single type so we
		 * don't have to compare against crc/non-crc types elsewhere.
		 */
		switch (be16_to_cpu(info->magic)) {
		case XFS_DA_NODE_MAGIC:
		case XFS_DA3_NODE_MAGIC:
			blk->magic = XFS_DA_NODE_MAGIC;
			node = (xfs_da_intnode_t *)info;
			dp->d_ops->node_hdr_from_disk(&nodehdr, node);
			btree = dp->d_ops->node_tree_p(node);
			blk->hashval = be32_to_cpu(btree[nodehdr.count - 1].hashval);
			if (forward)
				blk->index = 0;
			else
				blk->index = nodehdr.count - 1;
			blkno = be32_to_cpu(btree[blk->index].before);
			break;
		case XFS_ATTR_LEAF_MAGIC:
		case XFS_ATTR3_LEAF_MAGIC:
			blk->magic = XFS_ATTR_LEAF_MAGIC;
			ASSERT(level == path->active-1);
			blk->index = 0;
			blk->hashval = xfs_attr_leaf_lasthash(blk->bp, NULL);
			break;
		case XFS_DIR2_LEAFN_MAGIC:
		case XFS_DIR3_LEAFN_MAGIC:
			blk->magic = XFS_DIR2_LEAFN_MAGIC;
			ASSERT(level == path->active-1);
			blk->index = 0;
			blk->hashval = xfs_dir2_leafn_lasthash(args->dp,
							       blk->bp, NULL);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	*result = 0;
	return 0;
}
