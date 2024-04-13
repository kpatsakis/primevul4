xfs_da3_node_verify(
	struct xfs_buf		*bp)
{
	struct xfs_mount	*mp = bp->b_target->bt_mount;
	struct xfs_da_intnode	*hdr = bp->b_addr;
	struct xfs_da3_icnode_hdr ichdr;
	const struct xfs_dir_ops *ops;

	ops = xfs_dir_get_ops(mp, NULL);

	ops->node_hdr_from_disk(&ichdr, hdr);

	if (xfs_sb_version_hascrc(&mp->m_sb)) {
		struct xfs_da3_node_hdr *hdr3 = bp->b_addr;

		if (ichdr.magic != XFS_DA3_NODE_MAGIC)
			return false;

		if (!uuid_equal(&hdr3->info.uuid, &mp->m_sb.sb_uuid))
			return false;
		if (be64_to_cpu(hdr3->info.blkno) != bp->b_bn)
			return false;
	} else {
		if (ichdr.magic != XFS_DA_NODE_MAGIC)
			return false;
	}
	if (ichdr.level == 0)
		return false;
	if (ichdr.level > XFS_DA_NODE_MAXDEPTH)
		return false;
	if (ichdr.count == 0)
		return false;

	/*
	 * we don't know if the node is for and attribute or directory tree,
	 * so only fail if the count is outside both bounds
	 */
	if (ichdr.count > mp->m_dir_node_ents &&
	    ichdr.count > mp->m_attr_node_ents)
		return false;

	/* XXX: hash order check? */

	return true;
}
