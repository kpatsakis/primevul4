int crypto_init_ahash_spawn(struct crypto_ahash_spawn *spawn,
			    struct hash_alg_common *alg,
			    struct crypto_instance *inst)
{
	return crypto_init_spawn2(&spawn->base, &alg->base, inst,
				  &crypto_ahash_type);
}
