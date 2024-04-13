static struct mountpoint *lock_mount(struct path *path)
{
	struct vfsmount *mnt;
	struct dentry *dentry = path->dentry;
retry:
	mutex_lock(&dentry->d_inode->i_mutex);
	if (unlikely(cant_mount(dentry))) {
		mutex_unlock(&dentry->d_inode->i_mutex);
		return ERR_PTR(-ENOENT);
	}
	namespace_lock();
	mnt = lookup_mnt(path);
	if (likely(!mnt)) {
		struct mountpoint *mp = lookup_mountpoint(dentry);
		if (!mp)
			mp = new_mountpoint(dentry);
		if (IS_ERR(mp)) {
			namespace_unlock();
			mutex_unlock(&dentry->d_inode->i_mutex);
			return mp;
		}
		return mp;
	}
	namespace_unlock();
	mutex_unlock(&path->dentry->d_inode->i_mutex);
	path_put(path);
	path->mnt = mnt;
	dentry = path->dentry = dget(mnt->mnt_root);
	goto retry;
}
