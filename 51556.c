void audit_filter_inodes(struct task_struct *tsk, struct audit_context *ctx)
{
	struct audit_names *n;

	if (audit_pid && tsk->tgid == audit_pid)
		return;

	rcu_read_lock();

	list_for_each_entry(n, &ctx->names_list, list) {
		if (audit_filter_inode_name(tsk, n, ctx))
			break;
	}
	rcu_read_unlock();
}
