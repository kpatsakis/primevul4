static inline struct aes_ctx *blk_aes_ctx(struct crypto_blkcipher *tfm)
{
	return aes_ctx_common(crypto_blkcipher_ctx(tfm));
}
