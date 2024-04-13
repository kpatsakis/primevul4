static int lrw_decrypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		       struct scatterlist *src, unsigned int nbytes)
{
	struct camellia_lrw_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	be128 buf[2 * 4];
	struct lrw_crypt_req req = {
		.tbuf = buf,
		.tbuflen = sizeof(buf),

		.table_ctx = &ctx->lrw_table,
		.crypt_ctx = &ctx->camellia_ctx,
		.crypt_fn = decrypt_callback,
	};

	return lrw_crypt(desc, dst, src, nbytes, &req);
}
