static void perf_event_comm_event(struct perf_comm_event *comm_event)
{
	struct perf_cpu_context *cpuctx;
	struct perf_event_context *ctx;
	char comm[TASK_COMM_LEN];
	unsigned int size;
	struct pmu *pmu;
	int ctxn;

	memset(comm, 0, sizeof(comm));
	strlcpy(comm, comm_event->task->comm, sizeof(comm));
	size = ALIGN(strlen(comm)+1, sizeof(u64));

	comm_event->comm = comm;
	comm_event->comm_size = size;

	comm_event->event_id.header.size = sizeof(comm_event->event_id) + size;
	rcu_read_lock();
	list_for_each_entry_rcu(pmu, &pmus, entry) {
		cpuctx = get_cpu_ptr(pmu->pmu_cpu_context);
		if (cpuctx->unique_pmu != pmu)
			goto next;
		perf_event_comm_ctx(&cpuctx->ctx, comm_event);

		ctxn = pmu->task_ctx_nr;
		if (ctxn < 0)
			goto next;

		ctx = rcu_dereference(current->perf_event_ctxp[ctxn]);
		if (ctx)
			perf_event_comm_ctx(ctx, comm_event);
next:
		put_cpu_ptr(pmu->pmu_cpu_context);
	}
	rcu_read_unlock();
}
