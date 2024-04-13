static int xts_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct aesni_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[8];
	struct xts_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.tweak_ctx = aes_ctx(ctx->raw_tweak_ctx),
		.tweak_fn = aesni_xts_tweak,
		.crypt_ctx = aes_ctx(ctx->raw_crypt_ctx),
		.crypt_fn = lrw_xts_decrypt_callback,
	};
	int ret;

	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	kernel_fpu_begin();
	ret = xts_crypt(desc, dst, src, nbytes, &req);
	kernel_fpu_end();

	return ret;
}
