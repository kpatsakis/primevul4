static int btrfs_tmpfile(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct inode *inode = NULL;
	u64 objectid;
	u64 index;
	int ret = 0;

	/*
	 * 5 units required for adding orphan entry
	 */
	trans = btrfs_start_transaction(root, 5);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	ret = btrfs_find_free_ino(root, &objectid);
	if (ret)
		goto out;

	inode = btrfs_new_inode(trans, root, dir, NULL, 0,
				btrfs_ino(dir), objectid, mode, &index);
	if (IS_ERR(inode)) {
		ret = PTR_ERR(inode);
		inode = NULL;
		goto out;
	}

	inode->i_fop = &btrfs_file_operations;
	inode->i_op = &btrfs_file_inode_operations;

	inode->i_mapping->a_ops = &btrfs_aops;
	BTRFS_I(inode)->io_tree.ops = &btrfs_extent_io_ops;

	ret = btrfs_init_inode_security(trans, inode, dir, NULL);
	if (ret)
		goto out_inode;

	ret = btrfs_update_inode(trans, root, inode);
	if (ret)
		goto out_inode;
	ret = btrfs_orphan_add(trans, inode);
	if (ret)
		goto out_inode;

	/*
	 * We set number of links to 0 in btrfs_new_inode(), and here we set
	 * it to 1 because d_tmpfile() will issue a warning if the count is 0,
	 * through:
	 *
	 *    d_tmpfile() -> inode_dec_link_count() -> drop_nlink()
	 */
	set_nlink(inode, 1);
	unlock_new_inode(inode);
	d_tmpfile(dentry, inode);
	mark_inode_dirty(inode);

out:
	btrfs_end_transaction(trans, root);
	if (ret)
		iput(inode);
	btrfs_balance_delayed_items(root);
	btrfs_btree_balance_dirty(root);
	return ret;

out_inode:
	unlock_new_inode(inode);
	goto out;

}
