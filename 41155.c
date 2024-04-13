static int do_change_type(struct path *path, int flag)
{
	struct mount *m;
	struct mount *mnt = real_mount(path->mnt);
	int recurse = flag & MS_REC;
	int type;
	int err = 0;

	if (path->dentry != path->mnt->mnt_root)
		return -EINVAL;

	type = flags_to_propagation_type(flag);
	if (!type)
		return -EINVAL;

	down_write(&namespace_sem);
	if (type == MS_SHARED) {
		err = invent_group_ids(mnt, recurse);
		if (err)
			goto out_unlock;
	}

	br_write_lock(&vfsmount_lock);
	for (m = mnt; m; m = (recurse ? next_mnt(m, mnt) : NULL))
		change_mnt_propagation(m, type);
	br_write_unlock(&vfsmount_lock);

 out_unlock:
	up_write(&namespace_sem);
	return err;
}
