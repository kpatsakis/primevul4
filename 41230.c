static int do_make_slave(struct mount *mnt)
{
	struct mount *peer_mnt = mnt, *master = mnt->mnt_master;
	struct mount *slave_mnt;

	/*
	 * slave 'mnt' to a peer mount that has the
	 * same root dentry. If none is available then
	 * slave it to anything that is available.
	 */
	while ((peer_mnt = next_peer(peer_mnt)) != mnt &&
	       peer_mnt->mnt.mnt_root != mnt->mnt.mnt_root) ;

	if (peer_mnt == mnt) {
		peer_mnt = next_peer(mnt);
		if (peer_mnt == mnt)
			peer_mnt = NULL;
	}
	if (IS_MNT_SHARED(mnt) && list_empty(&mnt->mnt_share))
		mnt_release_group_id(mnt);

	list_del_init(&mnt->mnt_share);
	mnt->mnt_group_id = 0;

	if (peer_mnt)
		master = peer_mnt;

	if (master) {
		list_for_each_entry(slave_mnt, &mnt->mnt_slave_list, mnt_slave)
			slave_mnt->mnt_master = master;
		list_move(&mnt->mnt_slave, &master->mnt_slave_list);
		list_splice(&mnt->mnt_slave_list, master->mnt_slave_list.prev);
		INIT_LIST_HEAD(&mnt->mnt_slave_list);
	} else {
		struct list_head *p = &mnt->mnt_slave_list;
		while (!list_empty(p)) {
                        slave_mnt = list_first_entry(p,
					struct mount, mnt_slave);
			list_del_init(&slave_mnt->mnt_slave);
			slave_mnt->mnt_master = NULL;
		}
	}
	mnt->mnt_master = master;
	CLEAR_MNT_SHARED(mnt);
	return 0;
}
