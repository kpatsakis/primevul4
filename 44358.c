static inline void __bss_tim_clear(u8 *tim, u16 id)
{
	/*
	 * This format has been mandated by the IEEE specifications,
	 * so this line may not be changed to use the __clear_bit() format.
	 */
	tim[id / 8] &= ~(1 << (id % 8));
}
