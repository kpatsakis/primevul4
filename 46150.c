void btrfs_evict_inode(struct inode *inode)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_block_rsv *rsv, *global_rsv;
	int steal_from_global = 0;
	u64 min_size = btrfs_calc_trunc_metadata_size(root, 1);
	int ret;

	trace_btrfs_inode_evict(inode);

	evict_inode_truncate_pages(inode);

	if (inode->i_nlink &&
	    ((btrfs_root_refs(&root->root_item) != 0 &&
	      root->root_key.objectid != BTRFS_ROOT_TREE_OBJECTID) ||
	     btrfs_is_free_space_inode(inode)))
		goto no_delete;

	if (is_bad_inode(inode)) {
		btrfs_orphan_del(NULL, inode);
		goto no_delete;
	}
	/* do we really want it for ->i_nlink > 0 and zero btrfs_root_refs? */
	if (!special_file(inode->i_mode))
		btrfs_wait_ordered_range(inode, 0, (u64)-1);

	btrfs_free_io_failure_record(inode, 0, (u64)-1);

	if (root->fs_info->log_root_recovering) {
		BUG_ON(test_bit(BTRFS_INODE_HAS_ORPHAN_ITEM,
				 &BTRFS_I(inode)->runtime_flags));
		goto no_delete;
	}

	if (inode->i_nlink > 0) {
		BUG_ON(btrfs_root_refs(&root->root_item) != 0 &&
		       root->root_key.objectid != BTRFS_ROOT_TREE_OBJECTID);
		goto no_delete;
	}

	ret = btrfs_commit_inode_delayed_inode(inode);
	if (ret) {
		btrfs_orphan_del(NULL, inode);
		goto no_delete;
	}

	rsv = btrfs_alloc_block_rsv(root, BTRFS_BLOCK_RSV_TEMP);
	if (!rsv) {
		btrfs_orphan_del(NULL, inode);
		goto no_delete;
	}
	rsv->size = min_size;
	rsv->failfast = 1;
	global_rsv = &root->fs_info->global_block_rsv;

	btrfs_i_size_write(inode, 0);

	/*
	 * This is a bit simpler than btrfs_truncate since we've already
	 * reserved our space for our orphan item in the unlink, so we just
	 * need to reserve some slack space in case we add bytes and update
	 * inode item when doing the truncate.
	 */
	while (1) {
		ret = btrfs_block_rsv_refill(root, rsv, min_size,
					     BTRFS_RESERVE_FLUSH_LIMIT);

		/*
		 * Try and steal from the global reserve since we will
		 * likely not use this space anyway, we want to try as
		 * hard as possible to get this to work.
		 */
		if (ret)
			steal_from_global++;
		else
			steal_from_global = 0;
		ret = 0;

		/*
		 * steal_from_global == 0: we reserved stuff, hooray!
		 * steal_from_global == 1: we didn't reserve stuff, boo!
		 * steal_from_global == 2: we've committed, still not a lot of
		 * room but maybe we'll have room in the global reserve this
		 * time.
		 * steal_from_global == 3: abandon all hope!
		 */
		if (steal_from_global > 2) {
			btrfs_warn(root->fs_info,
				"Could not get space for a delete, will truncate on mount %d",
				ret);
			btrfs_orphan_del(NULL, inode);
			btrfs_free_block_rsv(root, rsv);
			goto no_delete;
		}

		trans = btrfs_join_transaction(root);
		if (IS_ERR(trans)) {
			btrfs_orphan_del(NULL, inode);
			btrfs_free_block_rsv(root, rsv);
			goto no_delete;
		}

		/*
		 * We can't just steal from the global reserve, we need tomake
		 * sure there is room to do it, if not we need to commit and try
		 * again.
		 */
		if (steal_from_global) {
			if (!btrfs_check_space_for_delayed_refs(trans, root))
				ret = btrfs_block_rsv_migrate(global_rsv, rsv,
							      min_size);
			else
				ret = -ENOSPC;
		}

		/*
		 * Couldn't steal from the global reserve, we have too much
		 * pending stuff built up, commit the transaction and try it
		 * again.
		 */
		if (ret) {
			ret = btrfs_commit_transaction(trans, root);
			if (ret) {
				btrfs_orphan_del(NULL, inode);
				btrfs_free_block_rsv(root, rsv);
				goto no_delete;
			}
			continue;
		} else {
			steal_from_global = 0;
		}

		trans->block_rsv = rsv;

		ret = btrfs_truncate_inode_items(trans, root, inode, 0, 0);
		if (ret != -ENOSPC && ret != -EAGAIN)
			break;

		trans->block_rsv = &root->fs_info->trans_block_rsv;
		btrfs_end_transaction(trans, root);
		trans = NULL;
		btrfs_btree_balance_dirty(root);
	}

	btrfs_free_block_rsv(root, rsv);

	/*
	 * Errors here aren't a big deal, it just means we leave orphan items
	 * in the tree.  They will be cleaned up on the next mount.
	 */
	if (ret == 0) {
		trans->block_rsv = root->orphan_block_rsv;
		btrfs_orphan_del(trans, inode);
	} else {
		btrfs_orphan_del(NULL, inode);
	}

	trans->block_rsv = &root->fs_info->trans_block_rsv;
	if (!(root == root->fs_info->tree_root ||
	      root->root_key.objectid == BTRFS_TREE_RELOC_OBJECTID))
		btrfs_return_ino(root, btrfs_ino(inode));

	btrfs_end_transaction(trans, root);
	btrfs_btree_balance_dirty(root);
no_delete:
	btrfs_remove_delayed_node(inode);
	clear_inode(inode);
	return;
}
