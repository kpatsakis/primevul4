static int crypto_xcbc_digest_final(struct shash_desc *pdesc, u8 *out)
{
	struct crypto_shash *parent = pdesc->tfm;
	unsigned long alignmask = crypto_shash_alignmask(parent);
	struct xcbc_tfm_ctx *tctx = crypto_shash_ctx(parent);
	struct xcbc_desc_ctx *ctx = shash_desc_ctx(pdesc);
	struct crypto_cipher *tfm = tctx->child;
	int bs = crypto_shash_blocksize(parent);
	u8 *consts = PTR_ALIGN(&tctx->ctx[0], alignmask + 1);
	u8 *odds = PTR_ALIGN(&ctx->ctx[0], alignmask + 1);
	u8 *prev = odds + bs;
	unsigned int offset = 0;

	if (ctx->len != bs) {
		unsigned int rlen;
		u8 *p = odds + ctx->len;

		*p = 0x80;
		p++;

		rlen = bs - ctx->len -1;
		if (rlen)
			memset(p, 0, rlen);

		offset += bs;
	}

	crypto_xor(prev, odds, bs);
	crypto_xor(prev, consts + offset, bs);

	crypto_cipher_encrypt_one(tfm, out, prev);

	return 0;
}
