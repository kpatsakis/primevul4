static int gfs2_fsync(struct file *file, loff_t start, loff_t end,
		      int datasync)
{
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	int sync_state = inode->i_state & I_DIRTY;
	struct gfs2_inode *ip = GFS2_I(inode);
	int ret = 0, ret1 = 0;

	if (mapping->nrpages) {
		ret1 = filemap_fdatawrite_range(mapping, start, end);
		if (ret1 == -EIO)
			return ret1;
	}

	if (!gfs2_is_jdata(ip))
		sync_state &= ~I_DIRTY_PAGES;
	if (datasync)
		sync_state &= ~I_DIRTY_SYNC;

	if (sync_state) {
		ret = sync_inode_metadata(inode, 1);
		if (ret)
			return ret;
		if (gfs2_is_jdata(ip))
			filemap_write_and_wait(mapping);
		gfs2_ail_flush(ip->i_gl, 1);
	}

	if (mapping->nrpages)
		ret = filemap_fdatawait_range(mapping, start, end);

	return ret ? ret : ret1;
}
