void mnt_pin(struct vfsmount *mnt)
{
	br_write_lock(&vfsmount_lock);
	real_mount(mnt)->mnt_pinned++;
	br_write_unlock(&vfsmount_lock);
}
