SYSCALL_DEFINE2(umount, char __user *, name, int, flags)
{
	struct path path;
	struct mount *mnt;
	int retval;
	int lookup_flags = 0;

	if (flags & ~(MNT_FORCE | MNT_DETACH | MNT_EXPIRE | UMOUNT_NOFOLLOW))
		return -EINVAL;

	if (!may_mount())
		return -EPERM;

	if (!(flags & UMOUNT_NOFOLLOW))
		lookup_flags |= LOOKUP_FOLLOW;

	retval = user_path_at(AT_FDCWD, name, lookup_flags, &path);
	if (retval)
		goto out;
	mnt = real_mount(path.mnt);
	retval = -EINVAL;
	if (path.dentry != path.mnt->mnt_root)
		goto dput_and_out;
	if (!check_mnt(mnt))
		goto dput_and_out;

	retval = do_umount(mnt, flags);
dput_and_out:
	/* we mustn't call path_put() as that would clear mnt_expiry_mark */
	dput(path.dentry);
	mntput_no_expire(mnt);
out:
	return retval;
}
