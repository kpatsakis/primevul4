int gfs2_open_common(struct inode *inode, struct file *file)
{
	struct gfs2_file *fp;
	int ret;

	if (S_ISREG(inode->i_mode)) {
		ret = generic_file_open(inode, file);
		if (ret)
			return ret;
	}

	fp = kzalloc(sizeof(struct gfs2_file), GFP_NOFS);
	if (!fp)
		return -ENOMEM;

	mutex_init(&fp->f_fl_mutex);

	gfs2_assert_warn(GFS2_SB(inode), !file->private_data);
	file->private_data = fp;
	return 0;
}
