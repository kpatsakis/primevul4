const char *crypto_attr_alg_name(struct rtattr *rta)
{
	struct crypto_attr_alg *alga;

	if (!rta)
		return ERR_PTR(-ENOENT);
	if (RTA_PAYLOAD(rta) < sizeof(*alga))
		return ERR_PTR(-EINVAL);
	if (rta->rta_type != CRYPTOA_ALG)
		return ERR_PTR(-EINVAL);

	alga = RTA_DATA(rta);
	alga->name[CRYPTO_MAX_ALG_NAME - 1] = 0;

	return alga->name;
}
