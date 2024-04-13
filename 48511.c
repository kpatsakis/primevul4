static int hmac_init(struct shash_desc *pdesc)
{
	return hmac_import(pdesc, crypto_shash_ctx_aligned(pdesc->tfm));
}
