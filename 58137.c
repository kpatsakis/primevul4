static int crypto_cbcmac_digest_update(struct shash_desc *pdesc, const u8 *p,
				       unsigned int len)
{
	struct crypto_shash *parent = pdesc->tfm;
	struct cbcmac_tfm_ctx *tctx = crypto_shash_ctx(parent);
	struct cbcmac_desc_ctx *ctx = shash_desc_ctx(pdesc);
	struct crypto_cipher *tfm = tctx->child;
	int bs = crypto_shash_digestsize(parent);
	u8 *dg = (u8 *)ctx + crypto_shash_descsize(parent) - bs;

	while (len > 0) {
		unsigned int l = min(len, bs - ctx->len);

		crypto_xor(dg + ctx->len, p, l);
		ctx->len +=l;
		len -= l;
		p += l;

		if (ctx->len == bs) {
			crypto_cipher_encrypt_one(tfm, dg, dg);
			ctx->len = 0;
		}
	}

	return 0;
}
