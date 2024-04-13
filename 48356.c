static int crypto_cmac_digest_update(struct shash_desc *pdesc, const u8 *p,
				     unsigned int len)
{
	struct crypto_shash *parent = pdesc->tfm;
	unsigned long alignmask = crypto_shash_alignmask(parent);
	struct cmac_tfm_ctx *tctx = crypto_shash_ctx(parent);
	struct cmac_desc_ctx *ctx = shash_desc_ctx(pdesc);
	struct crypto_cipher *tfm = tctx->child;
	int bs = crypto_shash_blocksize(parent);
	u8 *odds = PTR_ALIGN((void *)ctx->ctx, alignmask + 1);
	u8 *prev = odds + bs;

	/* checking the data can fill the block */
	if ((ctx->len + len) <= bs) {
		memcpy(odds + ctx->len, p, len);
		ctx->len += len;
		return 0;
	}

	/* filling odds with new data and encrypting it */
	memcpy(odds + ctx->len, p, bs - ctx->len);
	len -= bs - ctx->len;
	p += bs - ctx->len;

	crypto_xor(prev, odds, bs);
	crypto_cipher_encrypt_one(tfm, prev, prev);

	/* clearing the length */
	ctx->len = 0;

	/* encrypting the rest of data */
	while (len > bs) {
		crypto_xor(prev, p, bs);
		crypto_cipher_encrypt_one(tfm, prev, prev);
		p += bs;
		len -= bs;
	}

	/* keeping the surplus of blocksize */
	if (len) {
		memcpy(odds, p, len);
		ctx->len = len;
	}

	return 0;
}
