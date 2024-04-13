static void mcryptd_free(struct crypto_instance *inst)
{
	struct mcryptd_instance_ctx *ctx = crypto_instance_ctx(inst);
	struct hashd_instance_ctx *hctx = crypto_instance_ctx(inst);

	switch (inst->alg.cra_flags & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_AHASH:
		crypto_drop_shash(&hctx->spawn);
		kfree(ahash_instance(inst));
		return;
	default:
		crypto_drop_spawn(&ctx->spawn);
		kfree(inst);
	}
}
