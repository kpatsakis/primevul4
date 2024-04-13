static inline int tg3_40bit_overflow_test(struct tg3 *tp, dma_addr_t mapping,
					  int len)
{
#if defined(CONFIG_HIGHMEM) && (BITS_PER_LONG == 64)
	if (tg3_flag(tp, 40BIT_DMA_BUG))
		return ((u64) mapping + len) > DMA_BIT_MASK(40);
	return 0;
#else
	return 0;
#endif
}
