static enum audit_state audit_filter_syscall(struct task_struct *tsk,
					     struct audit_context *ctx,
					     struct list_head *list)
{
	struct audit_entry *e;
	enum audit_state state;

	if (audit_pid && tsk->tgid == audit_pid)
		return AUDIT_DISABLED;

	rcu_read_lock();
	if (!list_empty(list)) {
		list_for_each_entry_rcu(e, list, list) {
			if (audit_in_mask(&e->rule, ctx->major) &&
			    audit_filter_rules(tsk, &e->rule, ctx, NULL,
					       &state, false)) {
				rcu_read_unlock();
				ctx->current_state = state;
				return state;
			}
		}
	}
	rcu_read_unlock();
	return AUDIT_BUILD_CONTEXT;
}
