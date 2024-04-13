static void crypto_ccm_free(struct aead_instance *inst)
{
	struct ccm_instance_ctx *ctx = aead_instance_ctx(inst);

	crypto_drop_ahash(&ctx->mac);
	crypto_drop_skcipher(&ctx->ctr);
	kfree(inst);
}
