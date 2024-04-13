static struct crypto_alg *crypto_spawn_alg(struct crypto_spawn *spawn)
{
	struct crypto_alg *alg;
	struct crypto_alg *alg2;

	down_read(&crypto_alg_sem);
	alg = spawn->alg;
	alg2 = alg;
	if (alg2)
		alg2 = crypto_mod_get(alg2);
	up_read(&crypto_alg_sem);

	if (!alg2) {
		if (alg)
			crypto_shoot_alg(alg);
		return ERR_PTR(-EAGAIN);
	}

	return alg;
}
