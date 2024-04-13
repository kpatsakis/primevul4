xfs_da3_node_read(
	struct xfs_trans	*tp,
	struct xfs_inode	*dp,
	xfs_dablk_t		bno,
	xfs_daddr_t		mappedbno,
	struct xfs_buf		**bpp,
	int			which_fork)
{
	int			err;

	err = xfs_da_read_buf(tp, dp, bno, mappedbno, bpp,
					which_fork, &xfs_da3_node_buf_ops);
	if (!err && tp) {
		struct xfs_da_blkinfo	*info = (*bpp)->b_addr;
		int			type;

		switch (be16_to_cpu(info->magic)) {
		case XFS_DA_NODE_MAGIC:
		case XFS_DA3_NODE_MAGIC:
			type = XFS_BLFT_DA_NODE_BUF;
			break;
		case XFS_ATTR_LEAF_MAGIC:
		case XFS_ATTR3_LEAF_MAGIC:
			type = XFS_BLFT_ATTR_LEAF_BUF;
			break;
		case XFS_DIR2_LEAFN_MAGIC:
		case XFS_DIR3_LEAFN_MAGIC:
			type = XFS_BLFT_DIR_LEAFN_BUF;
			break;
		default:
			type = 0;
			ASSERT(0);
			break;
		}
		xfs_trans_buf_set_type(tp, *bpp, type);
	}
	return err;
}
