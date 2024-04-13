static void kvm_timer_init(void)
{
	int cpu;

	max_tsc_khz = tsc_khz;

	cpu_notifier_register_begin();
	if (!boot_cpu_has(X86_FEATURE_CONSTANT_TSC)) {
#ifdef CONFIG_CPU_FREQ
		struct cpufreq_policy policy;
		memset(&policy, 0, sizeof(policy));
		cpu = get_cpu();
		cpufreq_get_policy(&policy, cpu);
		if (policy.cpuinfo.max_freq)
			max_tsc_khz = policy.cpuinfo.max_freq;
		put_cpu();
#endif
		cpufreq_register_notifier(&kvmclock_cpufreq_notifier_block,
					  CPUFREQ_TRANSITION_NOTIFIER);
	}
	pr_debug("kvm: max_tsc_khz = %ld\n", max_tsc_khz);
	for_each_online_cpu(cpu)
		smp_call_function_single(cpu, tsc_khz_changed, NULL, 1);

	__register_hotcpu_notifier(&kvmclock_cpu_notifier_block);
	cpu_notifier_register_done();

}
