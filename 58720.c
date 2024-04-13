u64 hashlen_string(const void *salt, const char *name)
{
	unsigned long a = 0, x = 0, y = (unsigned long)salt;
	unsigned long adata, mask, len;
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;

	len = 0;
	goto inside;

	do {
		HASH_MIX(x, y, a);
		len += sizeof(unsigned long);
inside:
		a = load_unaligned_zeropad(name+len);
	} while (!has_zero(a, &adata, &constants));

	adata = prep_zero_mask(a, adata, &constants);
	mask = create_zero_mask(adata);
	x ^= a & zero_bytemask(mask);

	return hashlen_create(fold_hash(x, y), len + find_zero(mask));
}
