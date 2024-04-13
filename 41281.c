static inline int tg3_4g_overflow_test(dma_addr_t mapping, int len)
{
	u32 base = (u32) mapping & 0xffffffff;

	return (base > 0xffffdcc0) && (base + len + 8 < base);
}
