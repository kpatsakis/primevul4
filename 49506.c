struct crypto_alg *crypto_find_alg(const char *alg_name,
				   const struct crypto_type *frontend,
				   u32 type, u32 mask)
{
	struct crypto_alg *(*lookup)(const char *name, u32 type, u32 mask) =
		crypto_alg_mod_lookup;

	if (frontend) {
		type &= frontend->maskclear;
		mask &= frontend->maskclear;
		type |= frontend->type;
		mask |= frontend->maskset;

		if (frontend->lookup)
			lookup = frontend->lookup;
	}

	return lookup(alg_name, type, mask);
}
