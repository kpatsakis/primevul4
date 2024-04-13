static inline int crypto_is_test_larval(struct crypto_larval *larval)
{
	return larval->alg.cra_driver_name[0];
}
