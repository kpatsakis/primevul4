static int mcryptd_hash_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = crypto_tfm_alg_instance(tfm);
	struct hashd_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_shash_spawn *spawn = &ictx->spawn;
	struct mcryptd_hash_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_shash *hash;

	hash = crypto_spawn_shash(spawn);
	if (IS_ERR(hash))
		return PTR_ERR(hash);

	ctx->child = hash;
	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct mcryptd_hash_request_ctx) +
				 crypto_shash_descsize(hash));
	return 0;
}
