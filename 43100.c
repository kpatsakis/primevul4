xfs_da3_node_remove(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*drop_blk)
{
	struct xfs_da_intnode	*node;
	struct xfs_da3_icnode_hdr nodehdr;
	struct xfs_da_node_entry *btree;
	int			index;
	int			tmp;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_node_remove(state->args);

	node = drop_blk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);
	ASSERT(drop_blk->index < nodehdr.count);
	ASSERT(drop_blk->index >= 0);

	/*
	 * Copy over the offending entry, or just zero it out.
	 */
	index = drop_blk->index;
	btree = dp->d_ops->node_tree_p(node);
	if (index < nodehdr.count - 1) {
		tmp  = nodehdr.count - index - 1;
		tmp *= (uint)sizeof(xfs_da_node_entry_t);
		memmove(&btree[index], &btree[index + 1], tmp);
		xfs_trans_log_buf(state->args->trans, drop_blk->bp,
		    XFS_DA_LOGRANGE(node, &btree[index], tmp));
		index = nodehdr.count - 1;
	}
	memset(&btree[index], 0, sizeof(xfs_da_node_entry_t));
	xfs_trans_log_buf(state->args->trans, drop_blk->bp,
	    XFS_DA_LOGRANGE(node, &btree[index], sizeof(btree[index])));
	nodehdr.count -= 1;
	dp->d_ops->node_hdr_to_disk(node, &nodehdr);
	xfs_trans_log_buf(state->args->trans, drop_blk->bp,
	    XFS_DA_LOGRANGE(node, &node->hdr, dp->d_ops->node_hdr_size));

	/*
	 * Copy the last hash value from the block to propagate upwards.
	 */
	drop_blk->hashval = be32_to_cpu(btree[index - 1].hashval);
}
