static void normalize_task(struct rq *rq, struct task_struct *p)
{
	const struct sched_class *prev_class = p->sched_class;
	struct sched_attr attr = {
		.sched_policy = SCHED_NORMAL,
	};
	int old_prio = p->prio;
	int on_rq;

	on_rq = p->on_rq;
	if (on_rq)
		dequeue_task(rq, p, 0);
	__setscheduler(rq, p, &attr);
	if (on_rq) {
		enqueue_task(rq, p, 0);
		resched_task(rq->curr);
	}

	check_class_changed(rq, p, prev_class, old_prio);
}
