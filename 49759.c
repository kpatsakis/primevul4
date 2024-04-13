static inline struct aes_ctx *aes_ctx(struct crypto_tfm *tfm)
{
	return aes_ctx_common(crypto_tfm_ctx(tfm));
}
