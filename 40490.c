static struct crypto_alg *crypto_user_aead_alg(const char *name, u32 type,
					       u32 mask)
{
	int err;
	struct crypto_alg *alg;

	type &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	type |= CRYPTO_ALG_TYPE_AEAD;
	mask &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	mask |= CRYPTO_ALG_TYPE_MASK;

	for (;;) {
		alg = crypto_lookup_aead(name,  type, mask);
		if (!IS_ERR(alg))
			return alg;

		err = PTR_ERR(alg);
		if (err != -EAGAIN)
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
	}

	return ERR_PTR(err);
}
