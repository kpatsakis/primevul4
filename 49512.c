static void crypto_larval_destroy(struct crypto_alg *alg)
{
	struct crypto_larval *larval = (void *)alg;

	BUG_ON(!crypto_is_larval(alg));
	if (larval->adult)
		crypto_mod_put(larval->adult);
	kfree(larval);
}
