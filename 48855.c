static int gfs2_get_flags(struct file *filp, u32 __user *ptr)
{
	struct inode *inode = file_inode(filp);
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_holder gh;
	int error;
	u32 fsflags;

	gfs2_holder_init(ip->i_gl, LM_ST_SHARED, 0, &gh);
	error = gfs2_glock_nq(&gh);
	if (error)
		return error;

	fsflags = fsflags_cvt(gfs2_to_fsflags, ip->i_diskflags);
	if (!S_ISDIR(inode->i_mode) && ip->i_diskflags & GFS2_DIF_JDATA)
		fsflags |= FS_JOURNAL_DATA_FL;
	if (put_user(fsflags, ptr))
		error = -EFAULT;

	gfs2_glock_dq(&gh);
	gfs2_holder_uninit(&gh);
	return error;
}
