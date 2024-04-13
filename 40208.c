static bool __ptrace_detach(struct task_struct *tracer, struct task_struct *p)
{
	bool dead;

	__ptrace_unlink(p);

	if (p->exit_state != EXIT_ZOMBIE)
		return false;

	dead = !thread_group_leader(p);

	if (!dead && thread_group_empty(p)) {
		if (!same_thread_group(p->real_parent, tracer))
			dead = do_notify_parent(p, p->exit_signal);
		else if (ignoring_children(tracer->sighand)) {
			__wake_up_parent(p, tracer);
			dead = true;
		}
	}
	/* Mark it as in the process of being reaped. */
	if (dead)
		p->exit_state = EXIT_DEAD;
	return dead;
}
