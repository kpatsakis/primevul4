static inline void crypto_xor_byte(u8 *a, const u8 *b, unsigned int size)
{
	for (; size; size--)
		*a++ ^= *b++;
}
