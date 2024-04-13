static int gfs2_release(struct inode *inode, struct file *file)
{
	struct gfs2_inode *ip = GFS2_I(inode);

	kfree(file->private_data);
	file->private_data = NULL;

	if (!(file->f_mode & FMODE_WRITE))
		return 0;

	gfs2_rs_delete(ip, &inode->i_writecount);
	return 0;
}
