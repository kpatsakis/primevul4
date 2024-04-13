static void mntput_no_expire(struct mount *mnt)
{
put_again:
#ifdef CONFIG_SMP
	br_read_lock(&vfsmount_lock);
	if (likely(mnt->mnt_ns)) {
		/* shouldn't be the last one */
		mnt_add_count(mnt, -1);
		br_read_unlock(&vfsmount_lock);
		return;
	}
	br_read_unlock(&vfsmount_lock);

	br_write_lock(&vfsmount_lock);
	mnt_add_count(mnt, -1);
	if (mnt_get_count(mnt)) {
		br_write_unlock(&vfsmount_lock);
		return;
	}
#else
	mnt_add_count(mnt, -1);
	if (likely(mnt_get_count(mnt)))
		return;
	br_write_lock(&vfsmount_lock);
#endif
	if (unlikely(mnt->mnt_pinned)) {
		mnt_add_count(mnt, mnt->mnt_pinned + 1);
		mnt->mnt_pinned = 0;
		br_write_unlock(&vfsmount_lock);
		acct_auto_close_mnt(&mnt->mnt);
		goto put_again;
	}

	list_del(&mnt->mnt_instance);
	br_write_unlock(&vfsmount_lock);
	mntfree(mnt);
}
