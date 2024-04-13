void crypto_inc(u8 *a, unsigned int size)
{
	__be32 *b = (__be32 *)(a + size);
	u32 c;

	for (; size >= 4; size -= 4) {
		c = be32_to_cpu(*--b) + 1;
		*b = cpu_to_be32(c);
		if (c)
			return;
	}

	crypto_inc_byte(a, size);
}
