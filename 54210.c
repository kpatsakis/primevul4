void update_rq_clock(struct rq *rq)
{
	s64 delta;

	if (rq->skip_clock_update > 0)
		return;

	delta = sched_clock_cpu(cpu_of(rq)) - rq->clock;
	rq->clock += delta;
	update_rq_clock_task(rq, delta);
}
