static int xts_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct twofish_xts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[3];
	struct xts_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.tweak_ctx = &ctx->tweak_ctx,
		.tweak_fn = XTS_TWEAK_CAST(twofish_enc_blk),
		.crypt_ctx = &ctx->crypt_ctx,
		.crypt_fn = decrypt_callback,
	};

	return xts_crypt(desc, dst, src, nbytes, &req);
}
