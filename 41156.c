static int do_loopback(struct path *path, const char *old_name,
				int recurse)
{
	LIST_HEAD(umount_list);
	struct path old_path;
	struct mount *mnt = NULL, *old;
	int err;
	if (!old_name || !*old_name)
		return -EINVAL;
	err = kern_path(old_name, LOOKUP_FOLLOW|LOOKUP_AUTOMOUNT, &old_path);
	if (err)
		return err;

	err = -EINVAL;
	if (mnt_ns_loop(&old_path))
		goto out; 

	err = lock_mount(path);
	if (err)
		goto out;

	old = real_mount(old_path.mnt);

	err = -EINVAL;
	if (IS_MNT_UNBINDABLE(old))
		goto out2;

	if (!check_mnt(real_mount(path->mnt)) || !check_mnt(old))
		goto out2;

	if (recurse)
		mnt = copy_tree(old, old_path.dentry, 0);
	else
		mnt = clone_mnt(old, old_path.dentry, 0);

	if (IS_ERR(mnt)) {
		err = PTR_ERR(mnt);
		goto out;
	}

	err = graft_tree(mnt, path);
	if (err) {
		br_write_lock(&vfsmount_lock);
		umount_tree(mnt, 0, &umount_list);
		br_write_unlock(&vfsmount_lock);
	}
out2:
	unlock_mount(path);
	release_mounts(&umount_list);
out:
	path_put(&old_path);
	return err;
}
