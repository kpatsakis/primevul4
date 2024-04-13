int finish_automount(struct vfsmount *m, struct path *path)
{
	struct mount *mnt = real_mount(m);
	int err;
	/* The new mount record should have at least 2 refs to prevent it being
	 * expired before we get a chance to add it
	 */
	BUG_ON(mnt_get_count(mnt) < 2);

	if (m->mnt_sb == path->mnt->mnt_sb &&
	    m->mnt_root == path->dentry) {
		err = -ELOOP;
		goto fail;
	}

	err = do_add_mount(mnt, path, path->mnt->mnt_flags | MNT_SHRINKABLE);
	if (!err)
		return 0;
fail:
	/* remove m from any expiration list it may be on */
	if (!list_empty(&mnt->mnt_expire)) {
		down_write(&namespace_sem);
		br_write_lock(&vfsmount_lock);
		list_del_init(&mnt->mnt_expire);
		br_write_unlock(&vfsmount_lock);
		up_write(&namespace_sem);
	}
	mntput(m);
	mntput(m);
	return err;
}
