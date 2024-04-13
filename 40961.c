static void perf_branch_stack_sched_in(struct task_struct *prev,
				       struct task_struct *task)
{
	struct perf_cpu_context *cpuctx;
	struct pmu *pmu;
	unsigned long flags;

	/* no need to flush branch stack if not changing task */
	if (prev == task)
		return;

	local_irq_save(flags);

	rcu_read_lock();

	list_for_each_entry_rcu(pmu, &pmus, entry) {
		cpuctx = this_cpu_ptr(pmu->pmu_cpu_context);

		/*
		 * check if the context has at least one
		 * event using PERF_SAMPLE_BRANCH_STACK
		 */
		if (cpuctx->ctx.nr_branch_stack > 0
		    && pmu->flush_branch_stack) {

			pmu = cpuctx->ctx.pmu;

			perf_ctx_lock(cpuctx, cpuctx->task_ctx);

			perf_pmu_disable(pmu);

			pmu->flush_branch_stack();

			perf_pmu_enable(pmu);

			perf_ctx_unlock(cpuctx, cpuctx->task_ctx);
		}
	}

	rcu_read_unlock();

	local_irq_restore(flags);
}
