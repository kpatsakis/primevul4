 static int __init init(void)
 {
	int ret;

	if (!crypt_s390_func_available(KIMD_SHA_512, CRYPT_S390_MSA))
		return -EOPNOTSUPP;
	if ((ret = crypto_register_shash(&sha512_alg)) < 0)
		goto out;
	if ((ret = crypto_register_shash(&sha384_alg)) < 0)
		crypto_unregister_shash(&sha512_alg);
out:
	return ret;
}
