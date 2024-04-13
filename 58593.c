void d_delete(struct dentry * dentry)
{
	struct inode *inode;
	int isdir = 0;
	/*
	 * Are we the only user?
	 */
again:
	spin_lock(&dentry->d_lock);
	inode = dentry->d_inode;
	isdir = S_ISDIR(inode->i_mode);
	if (dentry->d_lockref.count == 1) {
		if (!spin_trylock(&inode->i_lock)) {
			spin_unlock(&dentry->d_lock);
			cpu_relax();
			goto again;
		}
		dentry->d_flags &= ~DCACHE_CANT_MOUNT;
		dentry_unlink_inode(dentry);
		fsnotify_nameremove(dentry, isdir);
		return;
	}

	if (!d_unhashed(dentry))
		__d_drop(dentry);

	spin_unlock(&dentry->d_lock);

	fsnotify_nameremove(dentry, isdir);
}
