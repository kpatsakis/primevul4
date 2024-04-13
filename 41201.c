void mnt_unpin(struct vfsmount *m)
{
	struct mount *mnt = real_mount(m);
	br_write_lock(&vfsmount_lock);
	if (mnt->mnt_pinned) {
		mnt_add_count(mnt, 1);
		mnt->mnt_pinned--;
	}
	br_write_unlock(&vfsmount_lock);
}
