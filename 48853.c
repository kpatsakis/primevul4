static ssize_t gfs2_file_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct gfs2_inode *ip = GFS2_I(file_inode(file));
	int ret;

	ret = gfs2_rs_alloc(ip);
	if (ret)
		return ret;

	gfs2_size_hint(file, iocb->ki_pos, iov_iter_count(from));

	if (file->f_flags & O_APPEND) {
		struct gfs2_holder gh;

		ret = gfs2_glock_nq_init(ip->i_gl, LM_ST_SHARED, 0, &gh);
		if (ret)
			return ret;
		gfs2_glock_dq_uninit(&gh);
	}

	return generic_file_write_iter(iocb, from);
}
