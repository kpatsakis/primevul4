static int chksum_digest(struct shash_desc *desc, const u8 *data,
			 unsigned int length, u8 *out)
{
	struct chksum_ctx *mctx = crypto_shash_ctx(desc->tfm);

	return __chksum_finup(&mctx->key, data, length, out);
}
