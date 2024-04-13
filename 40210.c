void __ptrace_unlink(struct task_struct *child)
{
	BUG_ON(!child->ptrace);

	child->ptrace = 0;
	child->parent = child->real_parent;
	list_del_init(&child->ptrace_entry);

	spin_lock(&child->sighand->siglock);

	/*
	 * Clear all pending traps and TRAPPING.  TRAPPING should be
	 * cleared regardless of JOBCTL_STOP_PENDING.  Do it explicitly.
	 */
	task_clear_jobctl_pending(child, JOBCTL_TRAP_MASK);
	task_clear_jobctl_trapping(child);

	/*
	 * Reinstate JOBCTL_STOP_PENDING if group stop is in effect and
	 * @child isn't dead.
	 */
	if (!(child->flags & PF_EXITING) &&
	    (child->signal->flags & SIGNAL_STOP_STOPPED ||
	     child->signal->group_stop_count)) {
		child->jobctl |= JOBCTL_STOP_PENDING;

		/*
		 * This is only possible if this thread was cloned by the
		 * traced task running in the stopped group, set the signal
		 * for the future reports.
		 * FIXME: we should change ptrace_init_task() to handle this
		 * case.
		 */
		if (!(child->jobctl & JOBCTL_STOP_SIGMASK))
			child->jobctl |= SIGSTOP;
	}

	/*
	 * If transition to TASK_STOPPED is pending or in TASK_TRACED, kick
	 * @child in the butt.  Note that @resume should be used iff @child
	 * is in TASK_TRACED; otherwise, we might unduly disrupt
	 * TASK_KILLABLE sleeps.
	 */
	if (child->jobctl & JOBCTL_STOP_PENDING || task_is_traced(child))
		ptrace_signal_wake_up(child, true);

	spin_unlock(&child->sighand->siglock);
}
