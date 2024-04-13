static struct crypto_instance *crypto_ccm_alloc(struct rtattr **tb)
{
	const char *cipher_name;
	char ctr_name[CRYPTO_MAX_ALG_NAME];
	char full_name[CRYPTO_MAX_ALG_NAME];

	cipher_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(cipher_name))
		return ERR_CAST(cipher_name);

	if (snprintf(ctr_name, CRYPTO_MAX_ALG_NAME, "ctr(%s)",
		     cipher_name) >= CRYPTO_MAX_ALG_NAME)
		return ERR_PTR(-ENAMETOOLONG);

	if (snprintf(full_name, CRYPTO_MAX_ALG_NAME, "ccm(%s)", cipher_name) >=
	    CRYPTO_MAX_ALG_NAME)
		return ERR_PTR(-ENAMETOOLONG);

	return crypto_ccm_alloc_common(tb, full_name, ctr_name, cipher_name);
}
