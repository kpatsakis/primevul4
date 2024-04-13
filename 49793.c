static int padlock_sha_export(struct shash_desc *desc, void *out)
{
	struct padlock_sha_desc *dctx = shash_desc_ctx(desc);

	return crypto_shash_export(&dctx->fallback, out);
}
