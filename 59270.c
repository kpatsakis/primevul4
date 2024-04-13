void perf_pmu_unregister(struct pmu *pmu)
{
	int remove_device;

	mutex_lock(&pmus_lock);
	remove_device = pmu_bus_running;
	list_del_rcu(&pmu->entry);
	mutex_unlock(&pmus_lock);

	/*
	 * We dereference the pmu list under both SRCU and regular RCU, so
	 * synchronize against both of those.
	 */
	synchronize_srcu(&pmus_srcu);
	synchronize_rcu();

	free_percpu(pmu->pmu_disable_count);
	if (pmu->type >= PERF_TYPE_MAX)
		idr_remove(&pmu_idr, pmu->type);
	if (remove_device) {
		if (pmu->nr_addr_filters)
			device_remove_file(pmu->dev, &dev_attr_nr_addr_filters);
		device_del(pmu->dev);
		put_device(pmu->dev);
	}
	free_pmu_context(pmu);
}
