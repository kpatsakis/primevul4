static int btrfs_mknod(struct inode *dir, struct dentry *dentry,
			umode_t mode, dev_t rdev)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct inode *inode = NULL;
	int err;
	int drop_inode = 0;
	u64 objectid;
	u64 index = 0;

	if (!new_valid_dev(rdev))
		return -EINVAL;

	/*
	 * 2 for inode item and ref
	 * 2 for dir items
	 * 1 for xattr if selinux is on
	 */
	trans = btrfs_start_transaction(root, 5);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	err = btrfs_find_free_ino(root, &objectid);
	if (err)
		goto out_unlock;

	inode = btrfs_new_inode(trans, root, dir, dentry->d_name.name,
				dentry->d_name.len, btrfs_ino(dir), objectid,
				mode, &index);
	if (IS_ERR(inode)) {
		err = PTR_ERR(inode);
		goto out_unlock;
	}

	/*
	* If the active LSM wants to access the inode during
	* d_instantiate it needs these. Smack checks to see
	* if the filesystem supports xattrs by looking at the
	* ops vector.
	*/
	inode->i_op = &btrfs_special_inode_operations;
	init_special_inode(inode, inode->i_mode, rdev);

	err = btrfs_init_inode_security(trans, inode, dir, &dentry->d_name);
	if (err)
		goto out_unlock_inode;

	err = btrfs_add_nondir(trans, dir, dentry, inode, 0, index);
	if (err) {
		goto out_unlock_inode;
	} else {
		btrfs_update_inode(trans, root, inode);
		unlock_new_inode(inode);
		d_instantiate(dentry, inode);
	}

out_unlock:
	btrfs_end_transaction(trans, root);
	btrfs_balance_delayed_items(root);
	btrfs_btree_balance_dirty(root);
	if (drop_inode) {
		inode_dec_link_count(inode);
		iput(inode);
	}
	return err;

out_unlock_inode:
	drop_inode = 1;
	unlock_new_inode(inode);
	goto out_unlock;

}
