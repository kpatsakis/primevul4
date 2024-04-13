xfs_da_read_buf(
	struct xfs_trans	*trans,
	struct xfs_inode	*dp,
	xfs_dablk_t		bno,
	xfs_daddr_t		mappedbno,
	struct xfs_buf		**bpp,
	int			whichfork,
	const struct xfs_buf_ops *ops)
{
	struct xfs_buf		*bp;
	struct xfs_buf_map	map;
	struct xfs_buf_map	*mapp;
	int			nmap;
	int			error;

	*bpp = NULL;
	mapp = &map;
	nmap = 1;
	error = xfs_dabuf_map(trans, dp, bno, mappedbno, whichfork,
				&mapp, &nmap);
	if (error) {
		/* mapping a hole is not an error, but we don't continue */
		if (error == -1)
			error = 0;
		goto out_free;
	}

	error = xfs_trans_read_buf_map(dp->i_mount, trans,
					dp->i_mount->m_ddev_targp,
					mapp, nmap, 0, &bp, ops);
	if (error)
		goto out_free;

	if (whichfork == XFS_ATTR_FORK)
		xfs_buf_set_ref(bp, XFS_ATTR_BTREE_REF);
	else
		xfs_buf_set_ref(bp, XFS_DIR_BTREE_REF);

	/*
	 * This verification code will be moved to a CRC verification callback
	 * function so just leave it here unchanged until then.
	 */
	{
		xfs_dir2_data_hdr_t	*hdr = bp->b_addr;
		xfs_dir2_free_t		*free = bp->b_addr;
		xfs_da_blkinfo_t	*info = bp->b_addr;
		uint			magic, magic1;
		struct xfs_mount	*mp = dp->i_mount;

		magic = be16_to_cpu(info->magic);
		magic1 = be32_to_cpu(hdr->magic);
		if (unlikely(
		    XFS_TEST_ERROR((magic != XFS_DA_NODE_MAGIC) &&
				   (magic != XFS_DA3_NODE_MAGIC) &&
				   (magic != XFS_ATTR_LEAF_MAGIC) &&
				   (magic != XFS_ATTR3_LEAF_MAGIC) &&
				   (magic != XFS_DIR2_LEAF1_MAGIC) &&
				   (magic != XFS_DIR3_LEAF1_MAGIC) &&
				   (magic != XFS_DIR2_LEAFN_MAGIC) &&
				   (magic != XFS_DIR3_LEAFN_MAGIC) &&
				   (magic1 != XFS_DIR2_BLOCK_MAGIC) &&
				   (magic1 != XFS_DIR3_BLOCK_MAGIC) &&
				   (magic1 != XFS_DIR2_DATA_MAGIC) &&
				   (magic1 != XFS_DIR3_DATA_MAGIC) &&
				   (free->hdr.magic !=
					cpu_to_be32(XFS_DIR2_FREE_MAGIC)) &&
				   (free->hdr.magic !=
					cpu_to_be32(XFS_DIR3_FREE_MAGIC)),
				mp, XFS_ERRTAG_DA_READ_BUF,
				XFS_RANDOM_DA_READ_BUF))) {
			trace_xfs_da_btree_corrupt(bp, _RET_IP_);
			XFS_CORRUPTION_ERROR("xfs_da_do_buf(2)",
					     XFS_ERRLEVEL_LOW, mp, info);
			error = XFS_ERROR(EFSCORRUPTED);
			xfs_trans_brelse(trans, bp);
			goto out_free;
		}
	}
	*bpp = bp;
out_free:
	if (mapp != &map)
		kmem_free(mapp);

	return error;
}
