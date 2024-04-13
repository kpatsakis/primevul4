static int mnt_is_readonly(struct vfsmount *mnt)
{
	if (mnt->mnt_sb->s_readonly_remount)
		return 1;
	/* Order wrt setting s_flags/s_readonly_remount in do_remount() */
	smp_rmb();
	return __mnt_is_readonly(mnt);
}
