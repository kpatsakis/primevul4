static int des3_ede_setkey(struct crypto_tfm *tfm, const u8 *key,
			   unsigned int keylen)
{
	struct des3_ede_ctx *dctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	u32 *expkey = dctx->expkey;

	return __des3_ede_setkey(expkey, flags, key, keylen);
}
