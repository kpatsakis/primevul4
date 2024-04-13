static int sha384_neon_final(struct shash_desc *desc, u8 *hash)
{
	u8 D[SHA512_DIGEST_SIZE];

	sha512_neon_final(desc, D);

	memcpy(hash, D, SHA384_DIGEST_SIZE);
	memset(D, 0, SHA512_DIGEST_SIZE);

	return 0;
}
