static inline void setbit128_bbe(void *b, int bit)
{
	__set_bit(bit ^ (0x80 -
#ifdef __BIG_ENDIAN
			 BITS_PER_LONG
#else
			 BITS_PER_BYTE
#endif
			), b);
}
