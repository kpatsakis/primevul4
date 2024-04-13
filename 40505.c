void crypto_put_default_rng(void)
{
	mutex_lock(&crypto_default_rng_lock);
	if (!--crypto_default_rng_refcnt) {
		crypto_free_rng(crypto_default_rng);
		crypto_default_rng = NULL;
	}
	mutex_unlock(&crypto_default_rng_lock);
}
