static int setkey_fallback_blk(struct crypto_tfm *tfm, const u8 *key,
		unsigned int len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
	unsigned int ret;

	sctx->fallback.blk->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
	sctx->fallback.blk->base.crt_flags |= (tfm->crt_flags &
			CRYPTO_TFM_REQ_MASK);

	ret = crypto_blkcipher_setkey(sctx->fallback.blk, key, len);
	if (ret) {
		tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
		tfm->crt_flags |= (sctx->fallback.blk->base.crt_flags &
				CRYPTO_TFM_RES_MASK);
	}
	return ret;
}
