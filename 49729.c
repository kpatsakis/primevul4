static int tgr128_final(struct shash_desc *desc, u8 * out)
{
	u8 D[64];

	tgr192_final(desc, D);
	memcpy(out, D, TGR128_DIGEST_SIZE);
	memzero_explicit(D, TGR192_DIGEST_SIZE);

	return 0;
}
