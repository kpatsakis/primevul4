static int xts_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct aesni_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);

	return glue_xts_crypt_128bit(&aesni_dec_xts, desc, dst, src, nbytes,
				     XTS_TWEAK_CAST(aesni_xts_tweak),
				     aes_ctx(ctx->raw_tweak_ctx),
				     aes_ctx(ctx->raw_crypt_ctx));
}
