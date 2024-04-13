void d_invalidate(struct dentry *dentry)
{
	/*
	 * If it's already been dropped, return OK.
	 */
	spin_lock(&dentry->d_lock);
	if (d_unhashed(dentry)) {
		spin_unlock(&dentry->d_lock);
		return;
	}
	spin_unlock(&dentry->d_lock);

	/* Negative dentries can be dropped without further checks */
	if (!dentry->d_inode) {
		d_drop(dentry);
		return;
	}

	for (;;) {
		struct detach_data data;

		data.mountpoint = NULL;
		INIT_LIST_HEAD(&data.select.dispose);
		data.select.start = dentry;
		data.select.found = 0;

		d_walk(dentry, &data, detach_and_collect, check_and_drop);

		if (!list_empty(&data.select.dispose))
			shrink_dentry_list(&data.select.dispose);
		else if (!data.mountpoint)
			return;

		if (data.mountpoint) {
			detach_mounts(data.mountpoint);
			dput(data.mountpoint);
		}
		cond_resched();
	}
}
