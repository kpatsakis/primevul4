static int do_remount(struct path *path, int flags, int mnt_flags,
		      void *data)
{
	int err;
	struct super_block *sb = path->mnt->mnt_sb;
	struct mount *mnt = real_mount(path->mnt);

	if (!check_mnt(mnt))
		return -EINVAL;

	if (path->dentry != path->mnt->mnt_root)
		return -EINVAL;

	/* Don't allow changing of locked mnt flags.
	 *
	 * No locks need to be held here while testing the various
	 * MNT_LOCK flags because those flags can never be cleared
	 * once they are set.
	 */
	if ((mnt->mnt.mnt_flags & MNT_LOCK_READONLY) &&
	    !(mnt_flags & MNT_READONLY)) {
		return -EPERM;
	}
	if ((mnt->mnt.mnt_flags & MNT_LOCK_NODEV) &&
	    !(mnt_flags & MNT_NODEV)) {
		/* Was the nodev implicitly added in mount? */
		if ((mnt->mnt_ns->user_ns != &init_user_ns) &&
		    !(sb->s_type->fs_flags & FS_USERNS_DEV_MOUNT)) {
			mnt_flags |= MNT_NODEV;
		} else {
			return -EPERM;
		}
	}
	if ((mnt->mnt.mnt_flags & MNT_LOCK_NOSUID) &&
	    !(mnt_flags & MNT_NOSUID)) {
		return -EPERM;
	}
	if ((mnt->mnt.mnt_flags & MNT_LOCK_NOEXEC) &&
	    !(mnt_flags & MNT_NOEXEC)) {
		return -EPERM;
	}
	if ((mnt->mnt.mnt_flags & MNT_LOCK_ATIME) &&
	    ((mnt->mnt.mnt_flags & MNT_ATIME_MASK) != (mnt_flags & MNT_ATIME_MASK))) {
		return -EPERM;
	}

	err = security_sb_remount(sb, data);
	if (err)
		return err;

	down_write(&sb->s_umount);
	if (flags & MS_BIND)
		err = change_mount_flags(path->mnt, flags);
	else if (!capable(CAP_SYS_ADMIN))
		err = -EPERM;
	else
		err = do_remount_sb(sb, flags, data, 0);
	if (!err) {
		lock_mount_hash();
		mnt_flags |= mnt->mnt.mnt_flags & ~MNT_USER_SETTABLE_MASK;
		mnt->mnt.mnt_flags = mnt_flags;
		touch_mnt_namespace(mnt->mnt_ns);
		unlock_mount_hash();
	}
	up_write(&sb->s_umount);
	return err;
}
