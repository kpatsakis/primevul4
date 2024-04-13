static int padlock_sha_update(struct shash_desc *desc,
			      const u8 *data, unsigned int length)
{
	struct padlock_sha_desc *dctx = shash_desc_ctx(desc);

	dctx->fallback.flags = desc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;
	return crypto_shash_update(&dctx->fallback, data, length);
}
