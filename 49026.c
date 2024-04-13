static int sha1_export(struct shash_desc *desc, void *out)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	memcpy(out, sctx, sizeof(*sctx));
	return 0;
}
