static int lrw_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct aesni_lrw_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[8];
	struct lrw_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.table_ctx = &ctx->lrw_table,
		.crypt_ctx = aes_ctx(ctx->raw_aes_ctx),
		.crypt_fn = lrw_xts_encrypt_callback,
	};
	int ret;

	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	kernel_fpu_begin();
	ret = lrw_crypt(desc, dst, src, nbytes, &req);
	kernel_fpu_end();

	return ret;
}
