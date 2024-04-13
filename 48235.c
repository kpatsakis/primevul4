static inline void crypto_inc_byte(u8 *a, unsigned int size)
{
	u8 *b = (a + size);
	u8 c;

	for (; size; size--) {
		c = *--b + 1;
		*b = c;
		if (c)
			break;
	}
}
