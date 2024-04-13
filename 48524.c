static inline int get_index128(be128 *block)
{
	int x;
	__be32 *p = (__be32 *) block;

	for (p += 3, x = 0; x < 128; p--, x += 32) {
		u32 val = be32_to_cpup(p);

		if (!~val)
			continue;

		return x + ffz(val);
	}

	return x;
}
