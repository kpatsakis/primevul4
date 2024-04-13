int crypto_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct aead_tfm *crt = crypto_aead_crt(tfm);
	int err;

	if (authsize > crypto_aead_alg(tfm)->maxauthsize)
		return -EINVAL;

	if (crypto_aead_alg(tfm)->setauthsize) {
		err = crypto_aead_alg(tfm)->setauthsize(crt->base, authsize);
		if (err)
			return err;
	}

	crypto_aead_crt(crt->base)->authsize = authsize;
	crt->authsize = authsize;
	return 0;
}
