static bool __follow_mount_rcu(struct nameidata *nd, struct path *path,
			       struct inode **inode, unsigned *seqp)
{
	for (;;) {
		struct mount *mounted;
		/*
		 * Don't forget we might have a non-mountpoint managed dentry
		 * that wants to block transit.
		 */
		switch (managed_dentry_rcu(path)) {
		case -ECHILD:
		default:
			return false;
		case -EISDIR:
			return true;
		case 0:
			break;
		}

		if (!d_mountpoint(path->dentry))
			return !(path->dentry->d_flags & DCACHE_NEED_AUTOMOUNT);

		mounted = __lookup_mnt(path->mnt, path->dentry);
		if (!mounted)
			break;
		path->mnt = &mounted->mnt;
		path->dentry = mounted->mnt.mnt_root;
		nd->flags |= LOOKUP_JUMPED;
		*seqp = read_seqcount_begin(&path->dentry->d_seq);
		/*
		 * Update the inode too. We don't need to re-check the
		 * dentry sequence number here after this d_inode read,
		 * because a mount-point is always pinned.
		 */
		*inode = path->dentry->d_inode;
	}
	return !read_seqretry(&mount_lock, nd->m_seq) &&
		!(path->dentry->d_flags & DCACHE_NEED_AUTOMOUNT);
}
