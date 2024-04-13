void __audit_free(struct task_struct *tsk)
{
	struct audit_context *context;

	context = audit_take_context(tsk, 0, 0);
	if (!context)
		return;

	/* Check for system calls that do not go through the exit
	 * function (e.g., exit_group), then free context block.
	 * We use GFP_ATOMIC here because we might be doing this
	 * in the context of the idle thread */
	/* that can happen only if we are called from do_exit() */
	if (context->in_syscall && context->current_state == AUDIT_RECORD_CONTEXT)
		audit_log_exit(context, tsk);
	if (!list_empty(&context->killed_trees))
		audit_kill_trees(&context->killed_trees);

	audit_free_context(context);
}
