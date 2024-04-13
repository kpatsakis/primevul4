void put_mnt_ns(struct mnt_namespace *ns)
{
	LIST_HEAD(umount_list);

	if (!atomic_dec_and_test(&ns->count))
		return;
	down_write(&namespace_sem);
	br_write_lock(&vfsmount_lock);
	umount_tree(ns->root, 0, &umount_list);
	br_write_unlock(&vfsmount_lock);
	up_write(&namespace_sem);
	release_mounts(&umount_list);
	free_mnt_ns(ns);
}
