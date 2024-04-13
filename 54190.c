sd_numa_init(struct sched_domain_topology_level *tl, int cpu)
{
	struct sched_domain *sd = *per_cpu_ptr(tl->data.sd, cpu);
	int level = tl->numa_level;
	int sd_weight = cpumask_weight(
			sched_domains_numa_masks[level][cpu_to_node(cpu)]);

	*sd = (struct sched_domain){
		.min_interval		= sd_weight,
		.max_interval		= 2*sd_weight,
		.busy_factor		= 32,
		.imbalance_pct		= 125,
		.cache_nice_tries	= 2,
		.busy_idx		= 3,
		.idle_idx		= 2,
		.newidle_idx		= 0,
		.wake_idx		= 0,
		.forkexec_idx		= 0,

		.flags			= 1*SD_LOAD_BALANCE
					| 1*SD_BALANCE_NEWIDLE
					| 0*SD_BALANCE_EXEC
					| 0*SD_BALANCE_FORK
					| 0*SD_BALANCE_WAKE
					| 0*SD_WAKE_AFFINE
					| 0*SD_SHARE_CPUPOWER
					| 0*SD_SHARE_PKG_RESOURCES
					| 1*SD_SERIALIZE
					| 0*SD_PREFER_SIBLING
					| 1*SD_NUMA
					| sd_local_flags(level)
					,
		.last_balance		= jiffies,
		.balance_interval	= sd_weight,
	};
	SD_INIT_NAME(sd, NUMA);
	sd->private = &tl->data;

	/*
	 * Ugly hack to pass state to sd_numa_mask()...
	 */
	sched_domains_curr_level = tl->numa_level;

	return sd;
}
