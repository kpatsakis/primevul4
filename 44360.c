static inline void __bss_tim_set(u8 *tim, u16 id)
{
	/*
	 * This format has been mandated by the IEEE specifications,
	 * so this line may not be changed to use the __set_bit() format.
	 */
	tim[id / 8] |= (1 << (id % 8));
}
