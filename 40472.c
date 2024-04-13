static int crypto_grab_nivcipher(struct crypto_skcipher_spawn *spawn,
				const char *name, u32 type, u32 mask)
{
	struct crypto_alg *alg;
	int err;

	type = crypto_skcipher_type(type);
	mask = crypto_skcipher_mask(mask)| CRYPTO_ALG_GENIV;

	alg = crypto_alg_mod_lookup(name, type, mask);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	err = crypto_init_spawn(&spawn->base, alg, spawn->base.inst, mask);
	crypto_mod_put(alg);
	return err;
}
