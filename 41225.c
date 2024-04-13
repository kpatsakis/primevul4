void umount_tree(struct mount *mnt, int propagate, struct list_head *kill)
{
	LIST_HEAD(tmp_list);
	struct mount *p;

	for (p = mnt; p; p = next_mnt(p, mnt))
		list_move(&p->mnt_hash, &tmp_list);

	if (propagate)
		propagate_umount(&tmp_list);

	list_for_each_entry(p, &tmp_list, mnt_hash) {
		list_del_init(&p->mnt_expire);
		list_del_init(&p->mnt_list);
		__touch_mnt_namespace(p->mnt_ns);
		p->mnt_ns = NULL;
		list_del_init(&p->mnt_child);
		if (mnt_has_parent(p)) {
			p->mnt_parent->mnt_ghosts++;
			dentry_reset_mounted(p->mnt_mountpoint);
		}
		change_mnt_propagation(p, MS_PRIVATE);
	}
	list_splice(&tmp_list, kill);
}
