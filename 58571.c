void __d_lookup_done(struct dentry *dentry)
{
	struct hlist_bl_head *b = in_lookup_hash(dentry->d_parent,
						 dentry->d_name.hash);
	hlist_bl_lock(b);
	dentry->d_flags &= ~DCACHE_PAR_LOOKUP;
	__hlist_bl_del(&dentry->d_u.d_in_lookup_hash);
	wake_up_all(dentry->d_wait);
	dentry->d_wait = NULL;
	hlist_bl_unlock(b);
	INIT_HLIST_NODE(&dentry->d_u.d_alias);
	INIT_LIST_HEAD(&dentry->d_lru);
}
