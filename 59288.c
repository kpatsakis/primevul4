static void remote_function(void *data)
{
	struct remote_function_call *tfc = data;
	struct task_struct *p = tfc->p;

	if (p) {
		/* -EAGAIN */
		if (task_cpu(p) != smp_processor_id())
			return;

		/*
		 * Now that we're on right CPU with IRQs disabled, we can test
		 * if we hit the right task without races.
		 */

		tfc->ret = -ESRCH; /* No such (running) process */
		if (p != current)
			return;
	}

	tfc->ret = tfc->func(tfc->info);
}
