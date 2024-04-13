static inline unsigned int shash_align_buffer_size(unsigned len,
						   unsigned long mask)
{
	typedef u8 __aligned_largest u8_aligned;
	return len + (mask & ~(__alignof__(u8_aligned) - 1));
}
