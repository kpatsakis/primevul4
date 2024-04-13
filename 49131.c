static int __init sha256_s390_init(void)
{
	int ret;

	if (!crypt_s390_func_available(KIMD_SHA_256, CRYPT_S390_MSA))
		return -EOPNOTSUPP;
	ret = crypto_register_shash(&sha256_alg);
	if (ret < 0)
		goto out;
	ret = crypto_register_shash(&sha224_alg);
	if (ret < 0)
		crypto_unregister_shash(&sha256_alg);
out:
	return ret;
}
