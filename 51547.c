void __audit_syscall_exit(int success, long return_code)
{
	struct task_struct *tsk = current;
	struct audit_context *context;

	if (success)
		success = AUDITSC_SUCCESS;
	else
		success = AUDITSC_FAILURE;

	context = audit_take_context(tsk, success, return_code);
	if (!context)
		return;

	if (context->in_syscall && context->current_state == AUDIT_RECORD_CONTEXT)
		audit_log_exit(context, tsk);

	context->in_syscall = 0;
	context->prio = context->state == AUDIT_RECORD_CONTEXT ? ~0ULL : 0;

	if (!list_empty(&context->killed_trees))
		audit_kill_trees(&context->killed_trees);

	audit_free_names(context);
	unroll_tree_refs(context, NULL, 0);
	audit_free_aux(context);
	context->aux = NULL;
	context->aux_pids = NULL;
	context->target_pid = 0;
	context->target_sid = 0;
	context->sockaddr_len = 0;
	context->type = 0;
	context->fds[0] = -1;
	if (context->state != AUDIT_RECORD_CONTEXT) {
		kfree(context->filterkey);
		context->filterkey = NULL;
	}
	tsk->audit_context = context;
}
