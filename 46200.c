static int btrfs_truncate(struct inode *inode)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_block_rsv *rsv;
	int ret = 0;
	int err = 0;
	struct btrfs_trans_handle *trans;
	u64 mask = root->sectorsize - 1;
	u64 min_size = btrfs_calc_trunc_metadata_size(root, 1);

	ret = btrfs_wait_ordered_range(inode, inode->i_size & (~mask),
				       (u64)-1);
	if (ret)
		return ret;

	/*
	 * Yes ladies and gentelment, this is indeed ugly.  The fact is we have
	 * 3 things going on here
	 *
	 * 1) We need to reserve space for our orphan item and the space to
	 * delete our orphan item.  Lord knows we don't want to have a dangling
	 * orphan item because we didn't reserve space to remove it.
	 *
	 * 2) We need to reserve space to update our inode.
	 *
	 * 3) We need to have something to cache all the space that is going to
	 * be free'd up by the truncate operation, but also have some slack
	 * space reserved in case it uses space during the truncate (thank you
	 * very much snapshotting).
	 *
	 * And we need these to all be seperate.  The fact is we can use alot of
	 * space doing the truncate, and we have no earthly idea how much space
	 * we will use, so we need the truncate reservation to be seperate so it
	 * doesn't end up using space reserved for updating the inode or
	 * removing the orphan item.  We also need to be able to stop the
	 * transaction and start a new one, which means we need to be able to
	 * update the inode several times, and we have no idea of knowing how
	 * many times that will be, so we can't just reserve 1 item for the
	 * entirety of the opration, so that has to be done seperately as well.
	 * Then there is the orphan item, which does indeed need to be held on
	 * to for the whole operation, and we need nobody to touch this reserved
	 * space except the orphan code.
	 *
	 * So that leaves us with
	 *
	 * 1) root->orphan_block_rsv - for the orphan deletion.
	 * 2) rsv - for the truncate reservation, which we will steal from the
	 * transaction reservation.
	 * 3) fs_info->trans_block_rsv - this will have 1 items worth left for
	 * updating the inode.
	 */
	rsv = btrfs_alloc_block_rsv(root, BTRFS_BLOCK_RSV_TEMP);
	if (!rsv)
		return -ENOMEM;
	rsv->size = min_size;
	rsv->failfast = 1;

	/*
	 * 1 for the truncate slack space
	 * 1 for updating the inode.
	 */
	trans = btrfs_start_transaction(root, 2);
	if (IS_ERR(trans)) {
		err = PTR_ERR(trans);
		goto out;
	}

	/* Migrate the slack space for the truncate to our reserve */
	ret = btrfs_block_rsv_migrate(&root->fs_info->trans_block_rsv, rsv,
				      min_size);
	BUG_ON(ret);

	/*
	 * So if we truncate and then write and fsync we normally would just
	 * write the extents that changed, which is a problem if we need to
	 * first truncate that entire inode.  So set this flag so we write out
	 * all of the extents in the inode to the sync log so we're completely
	 * safe.
	 */
	set_bit(BTRFS_INODE_NEEDS_FULL_SYNC, &BTRFS_I(inode)->runtime_flags);
	trans->block_rsv = rsv;

	while (1) {
		ret = btrfs_truncate_inode_items(trans, root, inode,
						 inode->i_size,
						 BTRFS_EXTENT_DATA_KEY);
		if (ret != -ENOSPC && ret != -EAGAIN) {
			err = ret;
			break;
		}

		trans->block_rsv = &root->fs_info->trans_block_rsv;
		ret = btrfs_update_inode(trans, root, inode);
		if (ret) {
			err = ret;
			break;
		}

		btrfs_end_transaction(trans, root);
		btrfs_btree_balance_dirty(root);

		trans = btrfs_start_transaction(root, 2);
		if (IS_ERR(trans)) {
			ret = err = PTR_ERR(trans);
			trans = NULL;
			break;
		}

		ret = btrfs_block_rsv_migrate(&root->fs_info->trans_block_rsv,
					      rsv, min_size);
		BUG_ON(ret);	/* shouldn't happen */
		trans->block_rsv = rsv;
	}

	if (ret == 0 && inode->i_nlink > 0) {
		trans->block_rsv = root->orphan_block_rsv;
		ret = btrfs_orphan_del(trans, inode);
		if (ret)
			err = ret;
	}

	if (trans) {
		trans->block_rsv = &root->fs_info->trans_block_rsv;
		ret = btrfs_update_inode(trans, root, inode);
		if (ret && !err)
			err = ret;

		ret = btrfs_end_transaction(trans, root);
		btrfs_btree_balance_dirty(root);
	}

out:
	btrfs_free_block_rsv(root, rsv);

	if (ret && !err)
		err = ret;

	return err;
}
