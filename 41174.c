static int lock_mount(struct path *path)
{
	struct vfsmount *mnt;
retry:
	mutex_lock(&path->dentry->d_inode->i_mutex);
	if (unlikely(cant_mount(path->dentry))) {
		mutex_unlock(&path->dentry->d_inode->i_mutex);
		return -ENOENT;
	}
	down_write(&namespace_sem);
	mnt = lookup_mnt(path);
	if (likely(!mnt))
		return 0;
	up_write(&namespace_sem);
	mutex_unlock(&path->dentry->d_inode->i_mutex);
	path_put(path);
	path->mnt = mnt;
	path->dentry = dget(mnt->mnt_root);
	goto retry;
}
