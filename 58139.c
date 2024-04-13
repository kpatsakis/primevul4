static int crypto_ccm_create(struct crypto_template *tmpl, struct rtattr **tb)
{
	const char *cipher_name;
	char ctr_name[CRYPTO_MAX_ALG_NAME];
	char mac_name[CRYPTO_MAX_ALG_NAME];
	char full_name[CRYPTO_MAX_ALG_NAME];

	cipher_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(cipher_name))
		return PTR_ERR(cipher_name);

	if (snprintf(ctr_name, CRYPTO_MAX_ALG_NAME, "ctr(%s)",
		     cipher_name) >= CRYPTO_MAX_ALG_NAME)
		return -ENAMETOOLONG;

	if (snprintf(mac_name, CRYPTO_MAX_ALG_NAME, "cbcmac(%s)",
		     cipher_name) >= CRYPTO_MAX_ALG_NAME)
		return -ENAMETOOLONG;

	if (snprintf(full_name, CRYPTO_MAX_ALG_NAME, "ccm(%s)", cipher_name) >=
	    CRYPTO_MAX_ALG_NAME)
		return -ENAMETOOLONG;

	return crypto_ccm_create_common(tmpl, tb, full_name, ctr_name,
					mac_name);
}
