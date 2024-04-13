static void shrink_submounts(struct mount *mnt, struct list_head *umounts)
{
	LIST_HEAD(graveyard);
	struct mount *m;

	/* extract submounts of 'mountpoint' from the expiration list */
	while (select_submounts(mnt, &graveyard)) {
		while (!list_empty(&graveyard)) {
			m = list_first_entry(&graveyard, struct mount,
						mnt_expire);
			touch_mnt_namespace(m->mnt_ns);
			umount_tree(m, 1, umounts);
		}
	}
}
