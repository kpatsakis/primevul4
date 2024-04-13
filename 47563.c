static inline void flush_tlb_early(void)
{
	__native_flush_tlb_global_irq_disabled();
}
