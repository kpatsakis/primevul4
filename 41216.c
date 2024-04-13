void release_mounts(struct list_head *head)
{
	struct mount *mnt;
	while (!list_empty(head)) {
		mnt = list_first_entry(head, struct mount, mnt_hash);
		list_del_init(&mnt->mnt_hash);
		if (mnt_has_parent(mnt)) {
			struct dentry *dentry;
			struct mount *m;

			br_write_lock(&vfsmount_lock);
			dentry = mnt->mnt_mountpoint;
			m = mnt->mnt_parent;
			mnt->mnt_mountpoint = mnt->mnt.mnt_root;
			mnt->mnt_parent = mnt;
			m->mnt_ghosts--;
			br_write_unlock(&vfsmount_lock);
			dput(dentry);
			mntput(&m->mnt);
		}
		mntput(&mnt->mnt);
	}
}
