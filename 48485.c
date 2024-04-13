static void crypto_rfc4543_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_rfc4543_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_aead(ctx->child);
	crypto_free_blkcipher(ctx->null);
}
