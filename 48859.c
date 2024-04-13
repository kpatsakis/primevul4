static int gfs2_open(struct inode *inode, struct file *file)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_holder i_gh;
	int error;
	bool need_unlock = false;

	if (S_ISREG(ip->i_inode.i_mode)) {
		error = gfs2_glock_nq_init(ip->i_gl, LM_ST_SHARED, LM_FLAG_ANY,
					   &i_gh);
		if (error)
			return error;
		need_unlock = true;
	}

	error = gfs2_open_common(inode, file);

	if (need_unlock)
		gfs2_glock_dq_uninit(&i_gh);

	return error;
}
