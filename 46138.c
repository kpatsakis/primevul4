static int btrfs_create(struct inode *dir, struct dentry *dentry,
			umode_t mode, bool excl)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct inode *inode = NULL;
	int drop_inode_on_err = 0;
	int err;
	u64 objectid;
	u64 index = 0;

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
	drop_inode_on_err = 1;
	/*
	* If the active LSM wants to access the inode during
	* d_instantiate it needs these. Smack checks to see
	* if the filesystem supports xattrs by looking at the
	* ops vector.
	*/
	inode->i_fop = &btrfs_file_operations;
	inode->i_op = &btrfs_file_inode_operations;
	inode->i_mapping->a_ops = &btrfs_aops;

	err = btrfs_init_inode_security(trans, inode, dir, &dentry->d_name);
	if (err)
		goto out_unlock_inode;

	err = btrfs_update_inode(trans, root, inode);
	if (err)
		goto out_unlock_inode;

	err = btrfs_add_nondir(trans, dir, dentry, inode, 0, index);
	if (err)
		goto out_unlock_inode;

	BTRFS_I(inode)->io_tree.ops = &btrfs_extent_io_ops;
	unlock_new_inode(inode);
	d_instantiate(dentry, inode);

out_unlock:
	btrfs_end_transaction(trans, root);
	if (err && drop_inode_on_err) {
		inode_dec_link_count(inode);
		iput(inode);
	}
	btrfs_balance_delayed_items(root);
	btrfs_btree_balance_dirty(root);
	return err;

out_unlock_inode:
	unlock_new_inode(inode);
	goto out_unlock;

}
