static int __init aes_s390_init(void)
{
	int ret;

	if (crypt_s390_func_available(KM_AES_128_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_128;
	if (crypt_s390_func_available(KM_AES_192_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_192;
	if (crypt_s390_func_available(KM_AES_256_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_256;

	if (!keylen_flag)
		return -EOPNOTSUPP;

	/* z9 109 and z9 BC/EC only support 128 bit key length */
	if (keylen_flag == AES_KEYLEN_128)
		pr_info("AES hardware acceleration is only available for"
			" 128-bit keys\n");

	ret = crypto_register_alg(&aes_alg);
	if (ret)
		goto aes_err;

	ret = crypto_register_alg(&ecb_aes_alg);
	if (ret)
		goto ecb_aes_err;

	ret = crypto_register_alg(&cbc_aes_alg);
	if (ret)
		goto cbc_aes_err;

	if (crypt_s390_func_available(KM_XTS_128_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KM_XTS_256_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4)) {
		ret = crypto_register_alg(&xts_aes_alg);
		if (ret)
			goto xts_aes_err;
		xts_aes_alg_reg = 1;
	}

	if (crypt_s390_func_available(KMCTR_AES_128_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KMCTR_AES_192_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KMCTR_AES_256_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4)) {
		ctrblk = (u8 *) __get_free_page(GFP_KERNEL);
		if (!ctrblk) {
			ret = -ENOMEM;
			goto ctr_aes_err;
		}
		ret = crypto_register_alg(&ctr_aes_alg);
		if (ret) {
			free_page((unsigned long) ctrblk);
			goto ctr_aes_err;
		}
		ctr_aes_alg_reg = 1;
	}

out:
	return ret;

ctr_aes_err:
	crypto_unregister_alg(&xts_aes_alg);
xts_aes_err:
	crypto_unregister_alg(&cbc_aes_alg);
cbc_aes_err:
	crypto_unregister_alg(&ecb_aes_alg);
ecb_aes_err:
	crypto_unregister_alg(&aes_alg);
aes_err:
	goto out;
}
