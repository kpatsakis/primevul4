static int audit_filter_inode_name(struct task_struct *tsk,
				   struct audit_names *n,
				   struct audit_context *ctx) {
	int h = audit_hash_ino((u32)n->ino);
	struct list_head *list = &audit_inode_hash[h];
	struct audit_entry *e;
	enum audit_state state;

	if (list_empty(list))
		return 0;

	list_for_each_entry_rcu(e, list, list) {
		if (audit_in_mask(&e->rule, ctx->major) &&
		    audit_filter_rules(tsk, &e->rule, ctx, n, &state, false)) {
			ctx->current_state = state;
			return 1;
		}
	}

	return 0;
}
