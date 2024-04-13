static int btrfs_unlink(struct inode *dir, struct dentry *dentry)
{
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct btrfs_trans_handle *trans;
	struct inode *inode = d_inode(dentry);
	int ret;

	trans = __unlink_start_trans(dir);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	btrfs_record_unlink_dir(trans, dir, d_inode(dentry), 0);

	ret = btrfs_unlink_inode(trans, root, dir, d_inode(dentry),
				 dentry->d_name.name, dentry->d_name.len);
	if (ret)
		goto out;

	if (inode->i_nlink == 0) {
		ret = btrfs_orphan_add(trans, inode);
		if (ret)
			goto out;
	}

out:
	btrfs_end_transaction(trans, root);
	btrfs_btree_balance_dirty(root);
	return ret;
}
