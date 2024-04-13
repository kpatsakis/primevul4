static inline struct audit_context *audit_take_context(struct task_struct *tsk,
						      int return_valid,
						      long return_code)
{
	struct audit_context *context = tsk->audit_context;

	if (!context)
		return NULL;
	context->return_valid = return_valid;

	/*
	 * we need to fix up the return code in the audit logs if the actual
	 * return codes are later going to be fixed up by the arch specific
	 * signal handlers
	 *
	 * This is actually a test for:
	 * (rc == ERESTARTSYS ) || (rc == ERESTARTNOINTR) ||
	 * (rc == ERESTARTNOHAND) || (rc == ERESTART_RESTARTBLOCK)
	 *
	 * but is faster than a bunch of ||
	 */
	if (unlikely(return_code <= -ERESTARTSYS) &&
	    (return_code >= -ERESTART_RESTARTBLOCK) &&
	    (return_code != -ENOIOCTLCMD))
		context->return_code = -EINTR;
	else
		context->return_code  = return_code;

	if (context->in_syscall && !context->dummy) {
		audit_filter_syscall(tsk, context, &audit_filter_list[AUDIT_FILTER_EXIT]);
		audit_filter_inodes(tsk, context);
	}

	tsk->audit_context = NULL;
	return context;
}
