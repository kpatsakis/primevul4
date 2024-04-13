xfs_da3_root_join(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*root_blk)
{
	struct xfs_da_intnode	*oldroot;
	struct xfs_da_args	*args;
	xfs_dablk_t		child;
	struct xfs_buf		*bp;
	struct xfs_da3_icnode_hdr oldroothdr;
	struct xfs_da_node_entry *btree;
	int			error;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_root_join(state->args);

	ASSERT(root_blk->magic == XFS_DA_NODE_MAGIC);

	args = state->args;
	oldroot = root_blk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&oldroothdr, oldroot);
	ASSERT(oldroothdr.forw == 0);
	ASSERT(oldroothdr.back == 0);

	/*
	 * If the root has more than one child, then don't do anything.
	 */
	if (oldroothdr.count > 1)
		return 0;

	/*
	 * Read in the (only) child block, then copy those bytes into
	 * the root block's buffer and free the original child block.
	 */
	btree = dp->d_ops->node_tree_p(oldroot);
	child = be32_to_cpu(btree[0].before);
	ASSERT(child != 0);
	error = xfs_da3_node_read(args->trans, dp, child, -1, &bp,
					     args->whichfork);
	if (error)
		return error;
	xfs_da_blkinfo_onlychild_validate(bp->b_addr, oldroothdr.level);

	/*
	 * This could be copying a leaf back into the root block in the case of
	 * there only being a single leaf block left in the tree. Hence we have
	 * to update the b_ops pointer as well to match the buffer type change
	 * that could occur. For dir3 blocks we also need to update the block
	 * number in the buffer header.
	 */
	memcpy(root_blk->bp->b_addr, bp->b_addr, state->blocksize);
	root_blk->bp->b_ops = bp->b_ops;
	xfs_trans_buf_copy_type(root_blk->bp, bp);
	if (oldroothdr.magic == XFS_DA3_NODE_MAGIC) {
		struct xfs_da3_blkinfo *da3 = root_blk->bp->b_addr;
		da3->blkno = cpu_to_be64(root_blk->bp->b_bn);
	}
	xfs_trans_log_buf(args->trans, root_blk->bp, 0, state->blocksize - 1);
	error = xfs_da_shrink_inode(args, child, bp);
	return(error);
}
