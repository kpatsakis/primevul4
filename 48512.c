static int hmac_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_shash *parent = __crypto_shash_cast(tfm);
	struct crypto_shash *hash;
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_shash_spawn *spawn = crypto_instance_ctx(inst);
	struct hmac_ctx *ctx = hmac_ctx(parent);

	hash = crypto_spawn_shash(spawn);
	if (IS_ERR(hash))
		return PTR_ERR(hash);

	parent->descsize = sizeof(struct shash_desc) +
			   crypto_shash_descsize(hash);

	ctx->hash = hash;
	return 0;
}
