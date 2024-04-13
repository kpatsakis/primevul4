static int xts_encrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct camellia_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[2 * 4];
	struct xts_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.tweak_ctx = &ctx->tweak_ctx,
		.tweak_fn = XTS_TWEAK_CAST(camellia_enc_blk),
		.crypt_ctx = &ctx->crypt_ctx,
		.crypt_fn = encrypt_callback,
	};

	return xts_crypt(desc, dst, src, nbytes, &req);
}
