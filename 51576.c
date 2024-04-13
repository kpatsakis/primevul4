static inline void handle_one(const struct inode *inode)
{
#ifdef CONFIG_AUDIT_TREE
	struct audit_context *context;
	struct audit_tree_refs *p;
	struct audit_chunk *chunk;
	int count;
	if (likely(hlist_empty(&inode->i_fsnotify_marks)))
		return;
	context = current->audit_context;
	p = context->trees;
	count = context->tree_count;
	rcu_read_lock();
	chunk = audit_tree_lookup(inode);
	rcu_read_unlock();
	if (!chunk)
		return;
	if (likely(put_tree_ref(context, chunk)))
		return;
	if (unlikely(!grow_tree_refs(context))) {
		pr_warn("out of memory, audit has lost a tree reference\n");
		audit_set_auditable(context);
		audit_put_chunk(chunk);
		unroll_tree_refs(context, p, count);
		return;
	}
	put_tree_ref(context, chunk);
#endif
}
