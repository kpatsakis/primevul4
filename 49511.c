struct crypto_larval *crypto_larval_alloc(const char *name, u32 type, u32 mask)
{
	struct crypto_larval *larval;

	larval = kzalloc(sizeof(*larval), GFP_KERNEL);
	if (!larval)
		return ERR_PTR(-ENOMEM);

	larval->mask = mask;
	larval->alg.cra_flags = CRYPTO_ALG_LARVAL | type;
	larval->alg.cra_priority = -1;
	larval->alg.cra_destroy = crypto_larval_destroy;

	strlcpy(larval->alg.cra_name, name, CRYPTO_MAX_ALG_NAME);
	init_completion(&larval->completion);

	return larval;
}
