static int __init des_s390_init(void)
{
	int ret;

	if (!crypt_s390_func_available(KM_DEA_ENCRYPT, CRYPT_S390_MSA) ||
	    !crypt_s390_func_available(KM_TDEA_192_ENCRYPT, CRYPT_S390_MSA))
		return -EOPNOTSUPP;

	ret = crypto_register_alg(&des_alg);
	if (ret)
		goto des_err;
	ret = crypto_register_alg(&ecb_des_alg);
	if (ret)
		goto ecb_des_err;
	ret = crypto_register_alg(&cbc_des_alg);
	if (ret)
		goto cbc_des_err;
	ret = crypto_register_alg(&des3_alg);
	if (ret)
		goto des3_err;
	ret = crypto_register_alg(&ecb_des3_alg);
	if (ret)
		goto ecb_des3_err;
	ret = crypto_register_alg(&cbc_des3_alg);
	if (ret)
		goto cbc_des3_err;

	if (crypt_s390_func_available(KMCTR_DEA_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KMCTR_TDEA_192_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4)) {
		ret = crypto_register_alg(&ctr_des_alg);
		if (ret)
			goto ctr_des_err;
		ret = crypto_register_alg(&ctr_des3_alg);
		if (ret)
			goto ctr_des3_err;
		ctrblk = (u8 *) __get_free_page(GFP_KERNEL);
		if (!ctrblk) {
			ret = -ENOMEM;
			goto ctr_mem_err;
		}
	}
out:
	return ret;

ctr_mem_err:
	crypto_unregister_alg(&ctr_des3_alg);
ctr_des3_err:
	crypto_unregister_alg(&ctr_des_alg);
ctr_des_err:
	crypto_unregister_alg(&cbc_des3_alg);
cbc_des3_err:
	crypto_unregister_alg(&ecb_des3_alg);
ecb_des3_err:
	crypto_unregister_alg(&des3_alg);
des3_err:
	crypto_unregister_alg(&cbc_des_alg);
cbc_des_err:
	crypto_unregister_alg(&ecb_des_alg);
ecb_des_err:
	crypto_unregister_alg(&des_alg);
des_err:
	goto out;
}
