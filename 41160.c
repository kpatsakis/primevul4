void drop_collected_mounts(struct vfsmount *mnt)
{
	LIST_HEAD(umount_list);
	down_write(&namespace_sem);
	br_write_lock(&vfsmount_lock);
	umount_tree(real_mount(mnt), 0, &umount_list);
	br_write_unlock(&vfsmount_lock);
	up_write(&namespace_sem);
	release_mounts(&umount_list);
}
