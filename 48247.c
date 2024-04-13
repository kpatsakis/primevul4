static inline int crypto_set_driver_name(struct crypto_alg *alg)
{
	static const char suffix[] = "-generic";
	char *driver_name = alg->cra_driver_name;
	int len;

	if (*driver_name)
		return 0;

	len = strlcpy(driver_name, alg->cra_name, CRYPTO_MAX_ALG_NAME);
	if (len + sizeof(suffix) > CRYPTO_MAX_ALG_NAME)
		return -ENAMETOOLONG;

	memcpy(driver_name + len, suffix, sizeof(suffix));
	return 0;
}
