xfs_da3_node_lasthash(
	struct xfs_inode	*dp,
	struct xfs_buf		*bp,
	int			*count)
{
	struct xfs_da_intnode	 *node;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr nodehdr;

	node = bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&nodehdr, node);
	if (count)
		*count = nodehdr.count;
	if (!nodehdr.count)
		return 0;
	btree = dp->d_ops->node_tree_p(node);
	return be32_to_cpu(btree[nodehdr.count - 1].hashval);
}
