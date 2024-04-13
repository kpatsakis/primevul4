struct crypto_blkcipher *cryptd_ablkcipher_child(struct cryptd_ablkcipher *tfm)
{
	struct cryptd_blkcipher_ctx *ctx = crypto_ablkcipher_ctx(&tfm->base);
	return ctx->child;
}
