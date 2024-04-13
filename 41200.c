void mnt_set_expiry(struct vfsmount *mnt, struct list_head *expiry_list)
{
	down_write(&namespace_sem);
	br_write_lock(&vfsmount_lock);

	list_add_tail(&real_mount(mnt)->mnt_expire, expiry_list);

	br_write_unlock(&vfsmount_lock);
	up_write(&namespace_sem);
}
