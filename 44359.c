static inline bool __bss_tim_get(u8 *tim, u16 id)
{
	/*
	 * This format has been mandated by the IEEE specifications,
	 * so this line may not be changed to use the test_bit() format.
	 */
	return tim[id / 8] & (1 << (id % 8));
}
