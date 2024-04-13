xfs_da3_node_add(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*oldblk,
	struct xfs_da_state_blk	*newblk)
{
	struct xfs_da_intnode	*node;
	struct xfs_da3_icnode_hdr nodehdr;
	struct xfs_da_node_entry *btree;
	int			tmp;
	struct xfs_inode	*dp = state->args->dp;

	trace_xfs_da_node_add(state->args);

	node = oldblk->bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);
	btree = dp->d_ops->node_tree_p(node);

	ASSERT(oldblk->index >= 0 && oldblk->index <= nodehdr.count);
	ASSERT(newblk->blkno != 0);
	if (state->args->whichfork == XFS_DATA_FORK)
		ASSERT(newblk->blkno >= state->mp->m_dirleafblk &&
		       newblk->blkno < state->mp->m_dirfreeblk);

	/*
	 * We may need to make some room before we insert the new node.
	 */
	tmp = 0;
	if (oldblk->index < nodehdr.count) {
		tmp = (nodehdr.count - oldblk->index) * (uint)sizeof(*btree);
		memmove(&btree[oldblk->index + 1], &btree[oldblk->index], tmp);
	}
	btree[oldblk->index].hashval = cpu_to_be32(newblk->hashval);
	btree[oldblk->index].before = cpu_to_be32(newblk->blkno);
	xfs_trans_log_buf(state->args->trans, oldblk->bp,
		XFS_DA_LOGRANGE(node, &btree[oldblk->index],
				tmp + sizeof(*btree)));

	nodehdr.count += 1;
	dp->d_ops->node_hdr_to_disk(node, &nodehdr);
	xfs_trans_log_buf(state->args->trans, oldblk->bp,
		XFS_DA_LOGRANGE(node, &node->hdr, dp->d_ops->node_hdr_size));

	/*
	 * Copy the last hash value from the oldblk to propagate upwards.
	 */
	oldblk->hashval = be32_to_cpu(btree[nodehdr.count - 1].hashval);
}
