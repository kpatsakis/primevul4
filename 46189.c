static int btrfs_setsize(struct inode *inode, struct iattr *attr)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;
	loff_t oldsize = i_size_read(inode);
	loff_t newsize = attr->ia_size;
	int mask = attr->ia_valid;
	int ret;

	/*
	 * The regular truncate() case without ATTR_CTIME and ATTR_MTIME is a
	 * special case where we need to update the times despite not having
	 * these flags set.  For all other operations the VFS set these flags
	 * explicitly if it wants a timestamp update.
	 */
	if (newsize != oldsize) {
		inode_inc_iversion(inode);
		if (!(mask & (ATTR_CTIME | ATTR_MTIME)))
			inode->i_ctime = inode->i_mtime =
				current_fs_time(inode->i_sb);
	}

	if (newsize > oldsize) {
		truncate_pagecache(inode, newsize);
		/*
		 * Don't do an expanding truncate while snapshoting is ongoing.
		 * This is to ensure the snapshot captures a fully consistent
		 * state of this file - if the snapshot captures this expanding
		 * truncation, it must capture all writes that happened before
		 * this truncation.
		 */
		wait_for_snapshot_creation(root);
		ret = btrfs_cont_expand(inode, oldsize, newsize);
		if (ret) {
			btrfs_end_write_no_snapshoting(root);
			return ret;
		}

		trans = btrfs_start_transaction(root, 1);
		if (IS_ERR(trans)) {
			btrfs_end_write_no_snapshoting(root);
			return PTR_ERR(trans);
		}

		i_size_write(inode, newsize);
		btrfs_ordered_update_i_size(inode, i_size_read(inode), NULL);
		ret = btrfs_update_inode(trans, root, inode);
		btrfs_end_write_no_snapshoting(root);
		btrfs_end_transaction(trans, root);
	} else {

		/*
		 * We're truncating a file that used to have good data down to
		 * zero. Make sure it gets into the ordered flush list so that
		 * any new writes get down to disk quickly.
		 */
		if (newsize == 0)
			set_bit(BTRFS_INODE_ORDERED_DATA_CLOSE,
				&BTRFS_I(inode)->runtime_flags);

		/*
		 * 1 for the orphan item we're going to add
		 * 1 for the orphan item deletion.
		 */
		trans = btrfs_start_transaction(root, 2);
		if (IS_ERR(trans))
			return PTR_ERR(trans);

		/*
		 * We need to do this in case we fail at _any_ point during the
		 * actual truncate.  Once we do the truncate_setsize we could
		 * invalidate pages which forces any outstanding ordered io to
		 * be instantly completed which will give us extents that need
		 * to be truncated.  If we fail to get an orphan inode down we
		 * could have left over extents that were never meant to live,
		 * so we need to garuntee from this point on that everything
		 * will be consistent.
		 */
		ret = btrfs_orphan_add(trans, inode);
		btrfs_end_transaction(trans, root);
		if (ret)
			return ret;

		/* we don't support swapfiles, so vmtruncate shouldn't fail */
		truncate_setsize(inode, newsize);

		/* Disable nonlocked read DIO to avoid the end less truncate */
		btrfs_inode_block_unlocked_dio(inode);
		inode_dio_wait(inode);
		btrfs_inode_resume_unlocked_dio(inode);

		ret = btrfs_truncate(inode);
		if (ret && inode->i_nlink) {
			int err;

			/*
			 * failed to truncate, disk_i_size is only adjusted down
			 * as we remove extents, so it should represent the true
			 * size of the inode, so reset the in memory size and
			 * delete our orphan entry.
			 */
			trans = btrfs_join_transaction(root);
			if (IS_ERR(trans)) {
				btrfs_orphan_del(NULL, inode);
				return ret;
			}
			i_size_write(inode, BTRFS_I(inode)->disk_i_size);
			err = btrfs_orphan_del(trans, inode);
			if (err)
				btrfs_abort_transaction(trans, root, err);
			btrfs_end_transaction(trans, root);
		}
	}

	return ret;
}
