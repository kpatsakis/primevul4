static struct crypto_instance *crypto_gcm_base_alloc(struct rtattr **tb)
{
	const char *ctr_name;
	const char *ghash_name;
	char full_name[CRYPTO_MAX_ALG_NAME];

	ctr_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(ctr_name))
		return ERR_CAST(ctr_name);

	ghash_name = crypto_attr_alg_name(tb[2]);
	if (IS_ERR(ghash_name))
		return ERR_CAST(ghash_name);

	if (snprintf(full_name, CRYPTO_MAX_ALG_NAME, "gcm_base(%s,%s)",
		     ctr_name, ghash_name) >= CRYPTO_MAX_ALG_NAME)
		return ERR_PTR(-ENAMETOOLONG);

	return crypto_gcm_alloc_common(tb, full_name, ctr_name, ghash_name);
}
