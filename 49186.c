static int md5_sparc64_export(struct shash_desc *desc, void *out)
{
	struct md5_state *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));

	return 0;
}
