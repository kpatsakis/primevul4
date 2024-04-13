static int fallback_blk_dec(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
	unsigned int ret;
	struct crypto_blkcipher *tfm;
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);

	tfm = desc->tfm;
	desc->tfm = sctx->fallback.blk;

	ret = crypto_blkcipher_decrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
	return ret;
}
