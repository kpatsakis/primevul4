static int sha384_final(struct shash_desc *desc, u8 *hash)
{
	u8 D[64];

	sha512_final(desc, D);

	memcpy(hash, D, 48);
	memzero_explicit(D, 64);

	return 0;
}
