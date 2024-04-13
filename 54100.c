static int __migrate_task(struct task_struct *p, int src_cpu, int dest_cpu)
{
	struct rq *rq_dest, *rq_src;
	int ret = 0;

	if (unlikely(!cpu_active(dest_cpu)))
		return ret;

	rq_src = cpu_rq(src_cpu);
	rq_dest = cpu_rq(dest_cpu);

	raw_spin_lock(&p->pi_lock);
	double_rq_lock(rq_src, rq_dest);
	/* Already moved. */
	if (task_cpu(p) != src_cpu)
		goto done;
	/* Affinity changed (again). */
	if (!cpumask_test_cpu(dest_cpu, tsk_cpus_allowed(p)))
		goto fail;

	/*
	 * If we're not on a rq, the next wake-up will ensure we're
	 * placed properly.
	 */
	if (p->on_rq) {
		dequeue_task(rq_src, p, 0);
		set_task_cpu(p, dest_cpu);
		enqueue_task(rq_dest, p, 0);
		check_preempt_curr(rq_dest, p, 0);
	}
done:
	ret = 1;
fail:
	double_rq_unlock(rq_src, rq_dest);
	raw_spin_unlock(&p->pi_lock);
	return ret;
}
