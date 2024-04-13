static int crypto_ccm_base_create(struct crypto_template *tmpl,
				  struct rtattr **tb)
{
	const char *ctr_name;
	const char *cipher_name;
	char full_name[CRYPTO_MAX_ALG_NAME];

	ctr_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(ctr_name))
		return PTR_ERR(ctr_name);

	cipher_name = crypto_attr_alg_name(tb[2]);
	if (IS_ERR(cipher_name))
		return PTR_ERR(cipher_name);

	if (snprintf(full_name, CRYPTO_MAX_ALG_NAME, "ccm_base(%s,%s)",
		     ctr_name, cipher_name) >= CRYPTO_MAX_ALG_NAME)
		return -ENAMETOOLONG;

	return crypto_ccm_create_common(tmpl, tb, full_name, ctr_name,
					cipher_name);
}
