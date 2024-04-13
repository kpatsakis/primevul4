static int __init sha1_s390_init(void)
{
	if (!crypt_s390_func_available(KIMD_SHA_1, CRYPT_S390_MSA))
		return -EOPNOTSUPP;
	return crypto_register_shash(&alg);
}
