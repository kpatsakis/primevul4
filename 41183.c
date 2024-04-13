int may_umount(struct vfsmount *mnt)
{
	int ret = 1;
	down_read(&namespace_sem);
	br_write_lock(&vfsmount_lock);
	if (propagate_mount_busy(real_mount(mnt), 2))
		ret = 0;
	br_write_unlock(&vfsmount_lock);
	up_read(&namespace_sem);
	return ret;
}
