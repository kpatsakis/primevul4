struct crypto_shash *mcryptd_ahash_child(struct mcryptd_ahash *tfm)
{
	struct mcryptd_hash_ctx *ctx = crypto_ahash_ctx(&tfm->base);

	return ctx->child;
}
