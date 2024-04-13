static int crypto_cbcmac_digest_init(struct shash_desc *pdesc)
{
	struct cbcmac_desc_ctx *ctx = shash_desc_ctx(pdesc);
	int bs = crypto_shash_digestsize(pdesc->tfm);
	u8 *dg = (u8 *)ctx + crypto_shash_descsize(pdesc->tfm) - bs;

	ctx->len = 0;
	memset(dg, 0, bs);

	return 0;
}
