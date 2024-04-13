static int encrypt_nss(
	struct crypto_instance *instance,
	const unsigned char *buf_in,
	const size_t buf_in_len,
	unsigned char *buf_out,
	size_t *buf_out_len)
{
	PK11Context*	crypt_context = NULL;
	SECItem		crypt_param;
	SECItem		*nss_sec_param = NULL;
	int		tmp1_outlen = 0;
	unsigned int	tmp2_outlen = 0;
	unsigned char	*salt = buf_out;
	unsigned char	*data = buf_out + SALT_SIZE;
	int		err = -1;

	if (!cipher_to_nss[instance->crypto_cipher_type]) {
		memcpy(buf_out, buf_in, buf_in_len);
		*buf_out_len = buf_in_len;
		return 0;
	}

	if (PK11_GenerateRandom (salt, SALT_SIZE) != SECSuccess) {
		log_printf(instance->log_level_security,
			"Failure to generate a random number %d",
			PR_GetError());
		goto out;
	}

	crypt_param.type = siBuffer;
	crypt_param.data = salt;
	crypt_param.len = SALT_SIZE;

	nss_sec_param = PK11_ParamFromIV (cipher_to_nss[instance->crypto_cipher_type],
					  &crypt_param);
	if (nss_sec_param == NULL) {
		log_printf(instance->log_level_security,
			   "Failure to set up PKCS11 param (err %d)",
			   PR_GetError());
		goto out;
	}

	/*
	 * Create cipher context for encryption
	 */
	crypt_context = PK11_CreateContextBySymKey (cipher_to_nss[instance->crypto_cipher_type],
						    CKA_ENCRYPT,
						    instance->nss_sym_key,
						    nss_sec_param);
	if (!crypt_context) {
		log_printf(instance->log_level_security,
			   "PK11_CreateContext failed (encrypt) crypt_type=%d (err %d)",
			   (int)cipher_to_nss[instance->crypto_cipher_type],
			   PR_GetError());
		goto out;
	}

	if (PK11_CipherOp(crypt_context, data,
			  &tmp1_outlen,
			  FRAME_SIZE_MAX - instance->crypto_header_size,
			  (unsigned char *)buf_in, buf_in_len) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_CipherOp failed (encrypt) crypt_type=%d (err %d)",
			   (int)cipher_to_nss[instance->crypto_cipher_type],
			   PR_GetError());
		goto out;
	}

	if (PK11_DigestFinal(crypt_context, data + tmp1_outlen,
			     &tmp2_outlen, FRAME_SIZE_MAX - tmp1_outlen) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_DigestFinal failed (encrypt) crypt_type=%d (err %d)",
			   (int)cipher_to_nss[instance->crypto_cipher_type],
			   PR_GetError());
		goto out;

	}

	*buf_out_len = tmp1_outlen + tmp2_outlen + SALT_SIZE;

	err = 0;

out:
	if (crypt_context) {
		PK11_DestroyContext(crypt_context, PR_TRUE);
	}
	if (nss_sec_param) {
		SECITEM_FreeItem(nss_sec_param, PR_TRUE);
	}
	return err;
}
