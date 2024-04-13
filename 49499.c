struct crypto_alg *crypto_alg_mod_lookup(const char *name, u32 type, u32 mask)
{
	struct crypto_alg *alg;
	struct crypto_alg *larval;
	int ok;

	if (!((type | mask) & CRYPTO_ALG_TESTED)) {
		type |= CRYPTO_ALG_TESTED;
		mask |= CRYPTO_ALG_TESTED;
	}

	larval = crypto_larval_lookup(name, type, mask);
	if (IS_ERR(larval) || !crypto_is_larval(larval))
		return larval;

	ok = crypto_probing_notify(CRYPTO_MSG_ALG_REQUEST, larval);

	if (ok == NOTIFY_STOP)
		alg = crypto_larval_wait(larval);
	else {
		crypto_mod_put(larval);
		alg = ERR_PTR(-ENOENT);
	}
	crypto_larval_kill(larval);
	return alg;
}
