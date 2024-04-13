static void dentry_free(struct dentry *dentry)
{
	WARN_ON(!hlist_unhashed(&dentry->d_u.d_alias));
	if (unlikely(dname_external(dentry))) {
		struct external_name *p = external_name(dentry);
		if (likely(atomic_dec_and_test(&p->u.count))) {
			call_rcu(&dentry->d_u.d_rcu, __d_free_external);
			return;
		}
	}
	/* if dentry was never visible to RCU, immediate free is OK */
	if (!(dentry->d_flags & DCACHE_RCUACCESS))
		__d_free(&dentry->d_u.d_rcu);
	else
		call_rcu(&dentry->d_u.d_rcu, __d_free);
}
