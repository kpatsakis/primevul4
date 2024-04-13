static int md5_sparc64_import(struct shash_desc *desc, const void *in)
{
	struct md5_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));

	return 0;
}
