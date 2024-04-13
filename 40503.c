int crypto_get_default_rng(void)
{
	struct crypto_rng *rng;
	int err;

	mutex_lock(&crypto_default_rng_lock);
	if (!crypto_default_rng) {
		rng = crypto_alloc_rng("stdrng", 0, 0);
		err = PTR_ERR(rng);
		if (IS_ERR(rng))
			goto unlock;

		err = crypto_rng_reset(rng, NULL, crypto_rng_seedsize(rng));
		if (err) {
			crypto_free_rng(rng);
			goto unlock;
		}

		crypto_default_rng = rng;
	}

	crypto_default_rng_refcnt++;
	err = 0;

unlock:
	mutex_unlock(&crypto_default_rng_lock);

	return err;
}
