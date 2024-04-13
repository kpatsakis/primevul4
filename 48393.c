static void cryptd_hash_exit_tfm(struct crypto_tfm *tfm)
{
	struct cryptd_hash_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_shash(ctx->child);
}
