void __d_drop(struct dentry *dentry)
{
	if (!d_unhashed(dentry)) {
		struct hlist_bl_head *b;
		/*
		 * Hashed dentries are normally on the dentry hashtable,
		 * with the exception of those newly allocated by
		 * d_obtain_alias, which are always IS_ROOT:
		 */
		if (unlikely(IS_ROOT(dentry)))
			b = &dentry->d_sb->s_anon;
		else
			b = d_hash(dentry->d_name.hash);

		hlist_bl_lock(b);
		__hlist_bl_del(&dentry->d_hash);
		dentry->d_hash.pprev = NULL;
		hlist_bl_unlock(b);
		/* After this call, in-progress rcu-walk path lookup will fail. */
		write_seqcount_invalidate(&dentry->d_seq);
	}
}
