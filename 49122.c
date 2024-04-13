static int __init ghash_mod_init(void)
{
	if (!crypt_s390_func_available(KIMD_GHASH,
				       CRYPT_S390_MSA | CRYPT_S390_MSA4))
		return -EOPNOTSUPP;

	return crypto_register_shash(&ghash_alg);
}
