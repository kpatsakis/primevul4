static int xts_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct serpent_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);

	return glue_xts_crypt_128bit(&serpent_dec_xts, desc, dst, src, nbytes,
				     XTS_TWEAK_CAST(__serpent_encrypt),
				     &ctx->tweak_ctx, &ctx->crypt_ctx);
}
