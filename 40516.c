static unsigned int crypto_shash_ctxsize(struct crypto_alg *alg, u32 type,
					 u32 mask)
{
	switch (mask & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_HASH_MASK:
		return sizeof(struct shash_desc *);
	}

	return 0;
}
