static int hmac_import(struct shash_desc *pdesc, const void *in)
{
	struct shash_desc *desc = shash_desc_ctx(pdesc);
	struct hmac_ctx *ctx = hmac_ctx(pdesc->tfm);

	desc->tfm = ctx->hash;
	desc->flags = pdesc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_shash_import(desc, in);
}
