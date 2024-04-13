static u64 do_task_delta_exec(struct task_struct *p, struct rq *rq)
{
	u64 ns = 0;

	if (task_current(rq, p)) {
		update_rq_clock(rq);
		ns = rq_clock_task(rq) - p->se.exec_start;
		if ((s64)ns < 0)
			ns = 0;
	}

	return ns;
}
