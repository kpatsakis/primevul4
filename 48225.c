struct crypto_alg *crypto_attr_alg2(struct rtattr *rta,
				    const struct crypto_type *frontend,
				    u32 type, u32 mask)
{
	const char *name;

	name = crypto_attr_alg_name(rta);
	if (IS_ERR(name))
		return ERR_CAST(name);

	return crypto_find_alg(name, frontend, type, mask);
}
