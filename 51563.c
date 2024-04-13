struct list_head *audit_killed_trees(void)
{
	struct audit_context *ctx = current->audit_context;
	if (likely(!ctx || !ctx->in_syscall))
		return NULL;
	return &ctx->killed_trees;
}
