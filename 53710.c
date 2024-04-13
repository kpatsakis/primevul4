bool fuse_write_update_size(struct inode *inode, loff_t pos)
{
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	bool ret = false;

	spin_lock(&fc->lock);
	fi->attr_version = ++fc->attr_version;
	if (pos > inode->i_size) {
		i_size_write(inode, pos);
		ret = true;
	}
	spin_unlock(&fc->lock);

	return ret;
}
