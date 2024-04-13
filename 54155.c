static int sched_cpu_inactive(struct notifier_block *nfb,
					unsigned long action, void *hcpu)
{
	unsigned long flags;
	long cpu = (long)hcpu;

	switch (action & ~CPU_TASKS_FROZEN) {
	case CPU_DOWN_PREPARE:
		set_cpu_active(cpu, false);

		/* explicitly allow suspend */
		if (!(action & CPU_TASKS_FROZEN)) {
			struct dl_bw *dl_b = dl_bw_of(cpu);
			bool overflow;
			int cpus;

			raw_spin_lock_irqsave(&dl_b->lock, flags);
			cpus = dl_bw_cpus(cpu);
			overflow = __dl_overflow(dl_b, cpus, 0, 0);
			raw_spin_unlock_irqrestore(&dl_b->lock, flags);

			if (overflow)
				return notifier_from_errno(-EBUSY);
		}
		return NOTIFY_OK;
	}

	return NOTIFY_DONE;
}
