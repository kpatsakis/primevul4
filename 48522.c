static void exit_tfm(struct crypto_tfm *tfm)
{
	struct priv *ctx = crypto_tfm_ctx(tfm);

	lrw_free_table(&ctx->table);
	crypto_free_cipher(ctx->child);
}
