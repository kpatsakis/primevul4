static inline void padlock_output_block(uint32_t *src,
		 	uint32_t *dst, size_t count)
{
	while (count--)
		*dst++ = swab32(*src++);
}
