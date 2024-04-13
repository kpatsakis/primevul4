int path_is_under(struct path *path1, struct path *path2)
{
	int res;
	br_read_lock(&vfsmount_lock);
	res = is_path_reachable(real_mount(path1->mnt), path1->dentry, path2);
	br_read_unlock(&vfsmount_lock);
	return res;
}
