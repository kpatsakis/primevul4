static int xts_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct twofish_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);

	return glue_xts_crypt_128bit(&twofish_enc_xts, desc, dst, src, nbytes,
				     XTS_TWEAK_CAST(twofish_enc_blk),
				     &ctx->tweak_ctx, &ctx->crypt_ctx);
}
