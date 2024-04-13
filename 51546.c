void __audit_syscall_entry(int major, unsigned long a1, unsigned long a2,
			   unsigned long a3, unsigned long a4)
{
	struct task_struct *tsk = current;
	struct audit_context *context = tsk->audit_context;
	enum audit_state     state;

	if (!context)
		return;

	BUG_ON(context->in_syscall || context->name_count);

	if (!audit_enabled)
		return;

	context->arch	    = syscall_get_arch();
	context->major      = major;
	context->argv[0]    = a1;
	context->argv[1]    = a2;
	context->argv[2]    = a3;
	context->argv[3]    = a4;

	state = context->state;
	context->dummy = !audit_n_rules;
	if (!context->dummy && state == AUDIT_BUILD_CONTEXT) {
		context->prio = 0;
		state = audit_filter_syscall(tsk, context, &audit_filter_list[AUDIT_FILTER_ENTRY]);
	}
	if (state == AUDIT_DISABLED)
		return;

	context->serial     = 0;
	context->ctime      = CURRENT_TIME;
	context->in_syscall = 1;
	context->current_state  = state;
	context->ppid       = 0;
}
