static inline struct crypto_rng *__crypto_rng_cast(struct crypto_tfm *tfm)
{
 	return container_of(tfm, struct crypto_rng, base);
 }
