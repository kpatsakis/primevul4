int ubifs_setattr(struct dentry *dentry, struct iattr *attr)
{
	int err;
	struct inode *inode = dentry->d_inode;
	struct ubifs_info *c = inode->i_sb->s_fs_info;

	dbg_gen("ino %lu, mode %#x, ia_valid %#x",
		inode->i_ino, inode->i_mode, attr->ia_valid);
	err = inode_change_ok(inode, attr);
	if (err)
		return err;

	err = dbg_check_synced_i_size(c, inode);
	if (err)
		return err;

	if ((attr->ia_valid & ATTR_SIZE) && attr->ia_size < inode->i_size)
		/* Truncation to a smaller size */
		err = do_truncation(c, inode, attr);
	else
		err = do_setattr(c, inode, attr);

	return err;
}
