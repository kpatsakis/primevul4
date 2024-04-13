static int hmac_finup(struct shash_desc *pdesc, const u8 *data,
		      unsigned int nbytes, u8 *out)
{

	struct crypto_shash *parent = pdesc->tfm;
	int ds = crypto_shash_digestsize(parent);
	int ss = crypto_shash_statesize(parent);
	char *opad = crypto_shash_ctx_aligned(parent) + ss;
	struct shash_desc *desc = shash_desc_ctx(pdesc);

	desc->flags = pdesc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_shash_finup(desc, data, nbytes, out) ?:
	       crypto_shash_import(desc, opad) ?:
	       crypto_shash_finup(desc, out, ds, out);
}
