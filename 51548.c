int audit_alloc(struct task_struct *tsk)
{
	struct audit_context *context;
	enum audit_state     state;
	char *key = NULL;

	if (likely(!audit_ever_enabled))
		return 0; /* Return if not auditing. */

	state = audit_filter_task(tsk, &key);
	if (state == AUDIT_DISABLED) {
		clear_tsk_thread_flag(tsk, TIF_SYSCALL_AUDIT);
		return 0;
	}

	if (!(context = audit_alloc_context(state))) {
		kfree(key);
		audit_log_lost("out of memory in audit_alloc");
		return -ENOMEM;
	}
	context->filterkey = key;

	tsk->audit_context  = context;
	set_tsk_thread_flag(tsk, TIF_SYSCALL_AUDIT);
	return 0;
}
