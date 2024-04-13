int propagate_umount(struct list_head *list)
{
	struct mount *mnt;

	list_for_each_entry(mnt, list, mnt_hash)
		__propagate_umount(mnt);
	return 0;
}
