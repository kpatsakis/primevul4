static inline struct hmac_ctx *hmac_ctx(struct crypto_shash *tfm)
{
	return align_ptr(crypto_shash_ctx_aligned(tfm) +
			 crypto_shash_statesize(tfm) * 2,
			 crypto_tfm_ctx_alignment());
}
