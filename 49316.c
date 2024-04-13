static int xts_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct cast6_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);

	return glue_xts_crypt_128bit(&cast6_enc_xts, desc, dst, src, nbytes,
				     XTS_TWEAK_CAST(__cast6_encrypt),
				     &ctx->tweak_ctx, &ctx->crypt_ctx);
}
