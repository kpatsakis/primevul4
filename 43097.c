xfs_da3_node_order(
	struct xfs_inode *dp,
	struct xfs_buf	*node1_bp,
	struct xfs_buf	*node2_bp)
{
	struct xfs_da_intnode	*node1;
	struct xfs_da_intnode	*node2;
	struct xfs_da_node_entry *btree1;
	struct xfs_da_node_entry *btree2;
	struct xfs_da3_icnode_hdr node1hdr;
	struct xfs_da3_icnode_hdr node2hdr;

	node1 = node1_bp->b_addr;
	node2 = node2_bp->b_addr;
	dp->d_ops->node_hdr_from_disk(&node1hdr, node1);
	dp->d_ops->node_hdr_from_disk(&node2hdr, node2);
	btree1 = dp->d_ops->node_tree_p(node1);
	btree2 = dp->d_ops->node_tree_p(node2);

	if (node1hdr.count > 0 && node2hdr.count > 0 &&
	    ((be32_to_cpu(btree2[0].hashval) < be32_to_cpu(btree1[0].hashval)) ||
	     (be32_to_cpu(btree2[node2hdr.count - 1].hashval) <
	      be32_to_cpu(btree1[node1hdr.count - 1].hashval)))) {
		return 1;
	}
	return 0;
}
