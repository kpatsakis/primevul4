static int do_flock(struct file *file, int cmd, struct file_lock *fl)
{
	struct gfs2_file *fp = file->private_data;
	struct gfs2_holder *fl_gh = &fp->f_fl_gh;
	struct gfs2_inode *ip = GFS2_I(file_inode(file));
	struct gfs2_glock *gl;
	unsigned int state;
	int flags;
	int error = 0;

	state = (fl->fl_type == F_WRLCK) ? LM_ST_EXCLUSIVE : LM_ST_SHARED;
	flags = (IS_SETLKW(cmd) ? 0 : LM_FLAG_TRY) | GL_EXACT | GL_NOCACHE;

	mutex_lock(&fp->f_fl_mutex);

	gl = fl_gh->gh_gl;
	if (gl) {
		if (fl_gh->gh_state == state)
			goto out;
		flock_lock_file_wait(file,
				     &(struct file_lock){.fl_type = F_UNLCK});
		gfs2_glock_dq_wait(fl_gh);
		gfs2_holder_reinit(state, flags, fl_gh);
	} else {
		error = gfs2_glock_get(GFS2_SB(&ip->i_inode), ip->i_no_addr,
				       &gfs2_flock_glops, CREATE, &gl);
		if (error)
			goto out;
		gfs2_holder_init(gl, state, flags, fl_gh);
		gfs2_glock_put(gl);
	}
	error = gfs2_glock_nq(fl_gh);
	if (error) {
		gfs2_holder_uninit(fl_gh);
		if (error == GLR_TRYFAILED)
			error = -EAGAIN;
	} else {
		error = flock_lock_file_wait(file, fl);
		gfs2_assert_warn(GFS2_SB(&ip->i_inode), !error);
	}

out:
	mutex_unlock(&fp->f_fl_mutex);
	return error;
}
