int crypto_has_alg(const char *name, u32 type, u32 mask)
{
	int ret = 0;
	struct crypto_alg *alg = crypto_alg_mod_lookup(name, type, mask);

	if (!IS_ERR(alg)) {
		crypto_mod_put(alg);
		ret = 1;
	}

	return ret;
}
