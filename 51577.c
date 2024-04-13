static void handle_path(const struct dentry *dentry)
{
#ifdef CONFIG_AUDIT_TREE
	struct audit_context *context;
	struct audit_tree_refs *p;
	const struct dentry *d, *parent;
	struct audit_chunk *drop;
	unsigned long seq;
	int count;

	context = current->audit_context;
	p = context->trees;
	count = context->tree_count;
retry:
	drop = NULL;
	d = dentry;
	rcu_read_lock();
	seq = read_seqbegin(&rename_lock);
	for(;;) {
		struct inode *inode = d_backing_inode(d);
		if (inode && unlikely(!hlist_empty(&inode->i_fsnotify_marks))) {
			struct audit_chunk *chunk;
			chunk = audit_tree_lookup(inode);
			if (chunk) {
				if (unlikely(!put_tree_ref(context, chunk))) {
					drop = chunk;
					break;
				}
			}
		}
		parent = d->d_parent;
		if (parent == d)
			break;
		d = parent;
	}
	if (unlikely(read_seqretry(&rename_lock, seq) || drop)) {  /* in this order */
		rcu_read_unlock();
		if (!drop) {
			/* just a race with rename */
			unroll_tree_refs(context, p, count);
			goto retry;
		}
		audit_put_chunk(drop);
		if (grow_tree_refs(context)) {
			/* OK, got more space */
			unroll_tree_refs(context, p, count);
			goto retry;
		}
		/* too bad */
		pr_warn("out of memory, audit has lost a tree reference\n");
		unroll_tree_refs(context, p, count);
		audit_set_auditable(context);
		return;
	}
	rcu_read_unlock();
#endif
}
