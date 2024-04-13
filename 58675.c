static struct vfsmount *debugfs_automount(struct path *path)
{
	debugfs_automount_t f;
	f = (debugfs_automount_t)path->dentry->d_fsdata;
	return f(path->dentry, d_inode(path->dentry)->i_private);
}
