static int __cpuinit console_cpu_notify(struct notifier_block *self,
	unsigned long action, void *hcpu)
{
	switch (action) {
	case CPU_ONLINE:
	case CPU_DEAD:
	case CPU_DYING:
	case CPU_DOWN_FAILED:
	case CPU_UP_CANCELED:
		console_lock();
		console_unlock();
	}
	return NOTIFY_OK;
}
