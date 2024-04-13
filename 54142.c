static void migrate_tasks(unsigned int dead_cpu)
{
	struct rq *rq = cpu_rq(dead_cpu);
	struct task_struct *next, *stop = rq->stop;
	int dest_cpu;

	/*
	 * Fudge the rq selection such that the below task selection loop
	 * doesn't get stuck on the currently eligible stop task.
	 *
	 * We're currently inside stop_machine() and the rq is either stuck
	 * in the stop_machine_cpu_stop() loop, or we're executing this code,
	 * either way we should never end up calling schedule() until we're
	 * done here.
	 */
	rq->stop = NULL;

	/*
	 * put_prev_task() and pick_next_task() sched
	 * class method both need to have an up-to-date
	 * value of rq->clock[_task]
	 */
	update_rq_clock(rq);

	for ( ; ; ) {
		/*
		 * There's this thread running, bail when that's the only
		 * remaining thread.
		 */
		if (rq->nr_running == 1)
			break;

		next = pick_next_task(rq);
		BUG_ON(!next);
		next->sched_class->put_prev_task(rq, next);

		/* Find suitable destination for @next, with force if needed. */
		dest_cpu = select_fallback_rq(dead_cpu, next);
		raw_spin_unlock(&rq->lock);

		__migrate_task(next, dead_cpu, dest_cpu);

		raw_spin_lock(&rq->lock);
	}

	rq->stop = stop;
}
