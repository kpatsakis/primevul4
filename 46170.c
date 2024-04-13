int btrfs_orphan_add(struct btrfs_trans_handle *trans, struct inode *inode)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_block_rsv *block_rsv = NULL;
	int reserve = 0;
	int insert = 0;
	int ret;

	if (!root->orphan_block_rsv) {
		block_rsv = btrfs_alloc_block_rsv(root, BTRFS_BLOCK_RSV_TEMP);
		if (!block_rsv)
			return -ENOMEM;
	}

	spin_lock(&root->orphan_lock);
	if (!root->orphan_block_rsv) {
		root->orphan_block_rsv = block_rsv;
	} else if (block_rsv) {
		btrfs_free_block_rsv(root, block_rsv);
		block_rsv = NULL;
	}

	if (!test_and_set_bit(BTRFS_INODE_HAS_ORPHAN_ITEM,
			      &BTRFS_I(inode)->runtime_flags)) {
#if 0
		/*
		 * For proper ENOSPC handling, we should do orphan
		 * cleanup when mounting. But this introduces backward
		 * compatibility issue.
		 */
		if (!xchg(&root->orphan_item_inserted, 1))
			insert = 2;
		else
			insert = 1;
#endif
		insert = 1;
		atomic_inc(&root->orphan_inodes);
	}

	if (!test_and_set_bit(BTRFS_INODE_ORPHAN_META_RESERVED,
			      &BTRFS_I(inode)->runtime_flags))
		reserve = 1;
	spin_unlock(&root->orphan_lock);

	/* grab metadata reservation from transaction handle */
	if (reserve) {
		ret = btrfs_orphan_reserve_metadata(trans, inode);
		BUG_ON(ret); /* -ENOSPC in reservation; Logic error? JDM */
	}

	/* insert an orphan item to track this unlinked/truncated file */
	if (insert >= 1) {
		ret = btrfs_insert_orphan_item(trans, root, btrfs_ino(inode));
		if (ret) {
			atomic_dec(&root->orphan_inodes);
			if (reserve) {
				clear_bit(BTRFS_INODE_ORPHAN_META_RESERVED,
					  &BTRFS_I(inode)->runtime_flags);
				btrfs_orphan_release_metadata(inode);
			}
			if (ret != -EEXIST) {
				clear_bit(BTRFS_INODE_HAS_ORPHAN_ITEM,
					  &BTRFS_I(inode)->runtime_flags);
				btrfs_abort_transaction(trans, root, ret);
				return ret;
			}
		}
		ret = 0;
	}

	/* insert an orphan item to track subvolume contains orphan files */
	if (insert >= 2) {
		ret = btrfs_insert_orphan_item(trans, root->fs_info->tree_root,
					       root->root_key.objectid);
		if (ret && ret != -EEXIST) {
			btrfs_abort_transaction(trans, root, ret);
			return ret;
		}
	}
	return 0;
}
