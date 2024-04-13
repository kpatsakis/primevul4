xfs_da3_node_read_verify(
	struct xfs_buf		*bp)
{
	struct xfs_mount	*mp = bp->b_target->bt_mount;
	struct xfs_da_blkinfo	*info = bp->b_addr;

	switch (be16_to_cpu(info->magic)) {
		case XFS_DA3_NODE_MAGIC:
			if (!xfs_verify_cksum(bp->b_addr, BBTOB(bp->b_length),
					      XFS_DA3_NODE_CRC_OFF))
				break;
			/* fall through */
		case XFS_DA_NODE_MAGIC:
			if (!xfs_da3_node_verify(bp))
				break;
			return;
		case XFS_ATTR_LEAF_MAGIC:
		case XFS_ATTR3_LEAF_MAGIC:
			bp->b_ops = &xfs_attr3_leaf_buf_ops;
			bp->b_ops->verify_read(bp);
			return;
		case XFS_DIR2_LEAFN_MAGIC:
		case XFS_DIR3_LEAFN_MAGIC:
			bp->b_ops = &xfs_dir3_leafn_buf_ops;
			bp->b_ops->verify_read(bp);
			return;
		default:
			break;
	}

	/* corrupt block */
	XFS_CORRUPTION_ERROR(__func__, XFS_ERRLEVEL_LOW, mp, bp->b_addr);
	xfs_buf_ioerror(bp, EFSCORRUPTED);
}
