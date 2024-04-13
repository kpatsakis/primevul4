static void dentry_reset_mounted(struct dentry *dentry)
{
	unsigned u;

	for (u = 0; u < HASH_SIZE; u++) {
		struct mount *p;

		list_for_each_entry(p, &mount_hashtable[u], mnt_hash) {
			if (p->mnt_mountpoint == dentry)
				return;
		}
	}
	spin_lock(&dentry->d_lock);
	dentry->d_flags &= ~DCACHE_MOUNTED;
	spin_unlock(&dentry->d_lock);
}
