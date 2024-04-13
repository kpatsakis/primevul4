static int hmac_export(struct shash_desc *pdesc, void *out)
{
	struct shash_desc *desc = shash_desc_ctx(pdesc);

	desc->flags = pdesc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_shash_export(desc, out);
}
