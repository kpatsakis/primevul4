int mnt_clone_write(struct vfsmount *mnt)
{
	/* superblock may be r/o */
	if (__mnt_is_readonly(mnt))
		return -EROFS;
	preempt_disable();
	mnt_inc_writers(real_mount(mnt));
	preempt_enable();
	return 0;
}
