static int sha512_ssse3_import(struct shash_desc *desc, const void *in)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));

	return 0;
}
