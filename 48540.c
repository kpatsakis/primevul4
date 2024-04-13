void mcryptd_flusher(struct work_struct *__work)
{
	struct	mcryptd_alg_cstate	*alg_cpu_state;
	struct	mcryptd_alg_state	*alg_state;
	struct	mcryptd_flush_list	*flist;
	int	cpu;

	cpu = smp_processor_id();
	alg_cpu_state = container_of(to_delayed_work(__work),
				     struct mcryptd_alg_cstate, flush);
	alg_state = alg_cpu_state->alg_state;
	if (alg_cpu_state->cpu != cpu)
		pr_debug("mcryptd error: work on cpu %d, should be cpu %d\n",
				cpu, alg_cpu_state->cpu);

	if (alg_cpu_state->flusher_engaged) {
		flist = per_cpu_ptr(mcryptd_flist, cpu);
		mutex_lock(&flist->lock);
		list_del(&alg_cpu_state->flush_list);
		alg_cpu_state->flusher_engaged = false;
		mutex_unlock(&flist->lock);
		alg_state->flusher(alg_cpu_state);
	}
}
