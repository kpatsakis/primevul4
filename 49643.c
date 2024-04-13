static int md5_export(struct shash_desc *desc, void *out)
{
	struct md5_state *ctx = shash_desc_ctx(desc);

	memcpy(out, ctx, sizeof(*ctx));
	return 0;
}
