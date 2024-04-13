static struct crypto_alg *crypto_user_skcipher_alg(const char *name, u32 type,
						   u32 mask)
{
	int err;
	struct crypto_alg *alg;

	type = crypto_skcipher_type(type);
	mask = crypto_skcipher_mask(mask);

	for (;;) {
		alg = crypto_lookup_skcipher(name,  type, mask);
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
