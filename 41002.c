int perf_pmu_register(struct pmu *pmu, char *name, int type)
{
	int cpu, ret;

	mutex_lock(&pmus_lock);
	ret = -ENOMEM;
	pmu->pmu_disable_count = alloc_percpu(int);
	if (!pmu->pmu_disable_count)
		goto unlock;

	pmu->type = -1;
	if (!name)
		goto skip_type;
	pmu->name = name;

	if (type < 0) {
		type = idr_alloc(&pmu_idr, pmu, PERF_TYPE_MAX, 0, GFP_KERNEL);
		if (type < 0) {
			ret = type;
			goto free_pdc;
		}
	}
	pmu->type = type;

	if (pmu_bus_running) {
		ret = pmu_dev_alloc(pmu);
		if (ret)
			goto free_idr;
	}

skip_type:
	pmu->pmu_cpu_context = find_pmu_context(pmu->task_ctx_nr);
	if (pmu->pmu_cpu_context)
		goto got_cpu_context;

	ret = -ENOMEM;
	pmu->pmu_cpu_context = alloc_percpu(struct perf_cpu_context);
	if (!pmu->pmu_cpu_context)
		goto free_dev;

	for_each_possible_cpu(cpu) {
		struct perf_cpu_context *cpuctx;

		cpuctx = per_cpu_ptr(pmu->pmu_cpu_context, cpu);
		__perf_event_init_context(&cpuctx->ctx);
		lockdep_set_class(&cpuctx->ctx.mutex, &cpuctx_mutex);
		lockdep_set_class(&cpuctx->ctx.lock, &cpuctx_lock);
		cpuctx->ctx.type = cpu_context;
		cpuctx->ctx.pmu = pmu;
		cpuctx->jiffies_interval = 1;
		INIT_LIST_HEAD(&cpuctx->rotation_list);
		cpuctx->unique_pmu = pmu;
	}

got_cpu_context:
	if (!pmu->start_txn) {
		if (pmu->pmu_enable) {
			/*
			 * If we have pmu_enable/pmu_disable calls, install
			 * transaction stubs that use that to try and batch
			 * hardware accesses.
			 */
			pmu->start_txn  = perf_pmu_start_txn;
			pmu->commit_txn = perf_pmu_commit_txn;
			pmu->cancel_txn = perf_pmu_cancel_txn;
		} else {
			pmu->start_txn  = perf_pmu_nop_void;
			pmu->commit_txn = perf_pmu_nop_int;
			pmu->cancel_txn = perf_pmu_nop_void;
		}
	}

	if (!pmu->pmu_enable) {
		pmu->pmu_enable  = perf_pmu_nop_void;
		pmu->pmu_disable = perf_pmu_nop_void;
	}

	if (!pmu->event_idx)
		pmu->event_idx = perf_event_idx_default;

	list_add_rcu(&pmu->entry, &pmus);
	ret = 0;
unlock:
	mutex_unlock(&pmus_lock);

	return ret;

free_dev:
	device_del(pmu->dev);
	put_device(pmu->dev);

free_idr:
	if (pmu->type >= PERF_TYPE_MAX)
		idr_remove(&pmu_idr, pmu->type);

free_pdc:
	free_percpu(pmu->pmu_disable_count);
	goto unlock;
}
