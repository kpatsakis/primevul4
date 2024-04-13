static int xts_fallback_encrypt(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	struct crypto_blkcipher *tfm;
	unsigned int ret;

	tfm = desc->tfm;
	desc->tfm = xts_ctx->fallback;

	ret = crypto_blkcipher_encrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
	return ret;
}
