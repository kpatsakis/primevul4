static int sha384_sparc64_final(struct shash_desc *desc, u8 *hash)
{
	u8 D[64];

	sha512_sparc64_final(desc, D);

	memcpy(hash, D, 48);
	memset(D, 0, 64);

	return 0;
}
