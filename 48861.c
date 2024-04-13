static int gfs2_readdir(struct file *file, struct dir_context *ctx)
{
	struct inode *dir = file->f_mapping->host;
	struct gfs2_inode *dip = GFS2_I(dir);
	struct gfs2_holder d_gh;
	int error;

	error = gfs2_glock_nq_init(dip->i_gl, LM_ST_SHARED, 0, &d_gh);
	if (error)
		return error;

	error = gfs2_dir_read(dir, ctx, &file->f_ra);

	gfs2_glock_dq_uninit(&d_gh);

	return error;
}
