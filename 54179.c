void sched_setnuma(struct task_struct *p, int nid)
{
	struct rq *rq;
	unsigned long flags;
	bool on_rq, running;

	rq = task_rq_lock(p, &flags);
	on_rq = p->on_rq;
	running = task_current(rq, p);

	if (on_rq)
		dequeue_task(rq, p, 0);
	if (running)
		p->sched_class->put_prev_task(rq, p);

	p->numa_preferred_nid = nid;

	if (running)
		p->sched_class->set_curr_task(rq);
	if (on_rq)
		enqueue_task(rq, p, 0);
	task_rq_unlock(rq, p, &flags);
}
