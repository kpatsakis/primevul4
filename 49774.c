static inline void padlock_reset_key(struct cword *cword)
{
	int cpu = raw_smp_processor_id();

	if (cword != per_cpu(paes_last_cword, cpu))
#ifndef CONFIG_X86_64
		asm volatile ("pushfl; popfl");
#else
		asm volatile ("pushfq; popfq");
#endif
}
