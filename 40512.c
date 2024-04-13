static int crypto_init_shash_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	switch (mask & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_HASH_MASK:
		return crypto_init_shash_ops_compat(tfm);
	}

	return -EINVAL;
}
