static int xts_fallback_setkey(struct crypto_tfm *tfm, const u8 *key,
				   unsigned int len)
{
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);
	unsigned int ret;

	xts_ctx->fallback->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
	xts_ctx->fallback->base.crt_flags |= (tfm->crt_flags &
			CRYPTO_TFM_REQ_MASK);

	ret = crypto_blkcipher_setkey(xts_ctx->fallback, key, len);
	if (ret) {
		tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
		tfm->crt_flags |= (xts_ctx->fallback->base.crt_flags &
				CRYPTO_TFM_RES_MASK);
	}
	return ret;
}
