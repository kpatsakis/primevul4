struct crypto_shash *cryptd_ahash_child(struct cryptd_ahash *tfm)
{
	struct cryptd_hash_ctx *ctx = crypto_ahash_ctx(&tfm->base);

	return ctx->child;
}
