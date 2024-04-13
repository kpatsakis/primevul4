static inline void padlock_store_cword(struct cword *cword)
{
	per_cpu(paes_last_cword, raw_smp_processor_id()) = cword;
}
