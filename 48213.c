static void crypto_fpu_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_fpu_ctx *ctx = crypto_tfm_ctx(tfm);
	crypto_free_blkcipher(ctx->child);
}
