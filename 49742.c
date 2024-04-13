static int wp384_final(struct shash_desc *desc, u8 *out)
{
	u8 D[64];

	wp512_final(desc, D);
	memcpy(out, D, WP384_DIGEST_SIZE);
	memzero_explicit(D, WP512_DIGEST_SIZE);

	return 0;
}
