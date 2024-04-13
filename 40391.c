struct crypto_ablkcipher *crypto_alloc_ablkcipher(const char *alg_name,
						  u32 type, u32 mask)
{
	struct crypto_tfm *tfm;
	int err;

	type = crypto_skcipher_type(type);
	mask = crypto_skcipher_mask(mask);

	for (;;) {
		struct crypto_alg *alg;

		alg = crypto_lookup_skcipher(alg_name, type, mask);
		if (IS_ERR(alg)) {
			err = PTR_ERR(alg);
			goto err;
		}

		tfm = __crypto_alloc_tfm(alg, type, mask);
		if (!IS_ERR(tfm))
			return __crypto_ablkcipher_cast(tfm);

		crypto_mod_put(alg);
		err = PTR_ERR(tfm);

err:
		if (err != -EAGAIN)
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
	}

	return ERR_PTR(err);
}
