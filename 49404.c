static int xts_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct serpent_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[SERPENT_PARALLEL_BLOCKS];
	struct crypt_priv crypt_ctx = {
		.ctx = &ctx->crypt_ctx,
		.fpu_enabled = false,
	};
	struct xts_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.tweak_ctx = &ctx->tweak_ctx,
		.tweak_fn = XTS_TWEAK_CAST(__serpent_encrypt),
		.crypt_ctx = &crypt_ctx,
		.crypt_fn = encrypt_callback,
	};
	int ret;

	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;
	ret = xts_crypt(desc, dst, src, nbytes, &req);
	serpent_fpu_end(crypt_ctx.fpu_enabled);

	return ret;
}
