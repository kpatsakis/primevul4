xfs_da3_node_unbalance(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*drop_blk,
	struct xfs_da_state_blk	*save_blk)
{
	struct xfs_da_intnode	*drop_node;
	struct xfs_da_intnode	*save_node;
	struct xfs_da_node_entry *drop_btree;
	struct xfs_da_node_entry *save_btree;
	struct xfs_da3_icnode_hdr drop_hdr;
	struct xfs_da3_icnode_hdr save_hdr;
	struct xfs_trans	*tp;
	int			sindex;
	int			tmp;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_node_unbalance(state->args);

	drop_node = drop_blk->bp->b_addr;
	save_node = save_blk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&drop_hdr, drop_node);
	dp->d_ops->node_hdr_from_disk(&save_hdr, save_node);
	drop_btree = dp->d_ops->node_tree_p(drop_node);
	save_btree = dp->d_ops->node_tree_p(save_node);
	tp = state->args->trans;

	/*
	 * If the dying block has lower hashvals, then move all the
	 * elements in the remaining block up to make a hole.
	 */
	if ((be32_to_cpu(drop_btree[0].hashval) <
			be32_to_cpu(save_btree[0].hashval)) ||
	    (be32_to_cpu(drop_btree[drop_hdr.count - 1].hashval) <
			be32_to_cpu(save_btree[save_hdr.count - 1].hashval))) {
		/* XXX: check this - is memmove dst correct? */
		tmp = save_hdr.count * sizeof(xfs_da_node_entry_t);
		memmove(&save_btree[drop_hdr.count], &save_btree[0], tmp);

		sindex = 0;
		xfs_trans_log_buf(tp, save_blk->bp,
			XFS_DA_LOGRANGE(save_node, &save_btree[0],
				(save_hdr.count + drop_hdr.count) *
						sizeof(xfs_da_node_entry_t)));
	} else {
		sindex = save_hdr.count;
		xfs_trans_log_buf(tp, save_blk->bp,
			XFS_DA_LOGRANGE(save_node, &save_btree[sindex],
				drop_hdr.count * sizeof(xfs_da_node_entry_t)));
	}

	/*
	 * Move all the B-tree elements from drop_blk to save_blk.
	 */
	tmp = drop_hdr.count * (uint)sizeof(xfs_da_node_entry_t);
	memcpy(&save_btree[sindex], &drop_btree[0], tmp);
	save_hdr.count += drop_hdr.count;

	dp->d_ops->node_hdr_to_disk(save_node, &save_hdr);
	xfs_trans_log_buf(tp, save_blk->bp,
		XFS_DA_LOGRANGE(save_node, &save_node->hdr,
				dp->d_ops->node_hdr_size));

	/*
	 * Save the last hashval in the remaining block for upward propagation.
	 */
	save_blk->hashval = be32_to_cpu(save_btree[save_hdr.count - 1].hashval);
}
