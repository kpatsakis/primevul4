void kern_unmount(struct vfsmount *mnt)
{
	/* release long term mount so mount point can be released */
	if (!IS_ERR_OR_NULL(mnt)) {
		br_write_lock(&vfsmount_lock);
		real_mount(mnt)->mnt_ns = NULL;
		br_write_unlock(&vfsmount_lock);
		mntput(mnt);
	}
}
