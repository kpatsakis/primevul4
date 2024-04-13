static int cpuset_cpu_active(struct notifier_block *nfb, unsigned long action,
			     void *hcpu)
{
	switch (action) {
	case CPU_ONLINE_FROZEN:
	case CPU_DOWN_FAILED_FROZEN:

		/*
		 * num_cpus_frozen tracks how many CPUs are involved in suspend
		 * resume sequence. As long as this is not the last online
		 * operation in the resume sequence, just build a single sched
		 * domain, ignoring cpusets.
		 */
		num_cpus_frozen--;
		if (likely(num_cpus_frozen)) {
			partition_sched_domains(1, NULL, NULL);
			break;
		}

		/*
		 * This is the last CPU online operation. So fall through and
		 * restore the original sched domains by considering the
		 * cpuset configurations.
		 */

	case CPU_ONLINE:
	case CPU_DOWN_FAILED:
		cpuset_update_active_cpus(true);
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}
