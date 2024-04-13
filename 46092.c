static inline u64 nsec_to_cycles(u64 nsec)
{
	u64 ret;

	WARN_ON(preemptible());
	if (kvm_tsc_changes_freq())
		printk_once(KERN_WARNING
		 "kvm: unreliable cycle conversion on adjustable rate TSC\n");
	ret = nsec * __get_cpu_var(cpu_tsc_khz);
	do_div(ret, USEC_PER_SEC);
	return ret;
}
