static int decrypt_nss (
	struct crypto_instance *instance,
	unsigned char *buf,
	int *buf_len)
{
	PK11Context*	decrypt_context = NULL;
	SECItem		decrypt_param;
	int		tmp1_outlen = 0;
	unsigned int	tmp2_outlen = 0;
	unsigned char	*salt = buf;
	unsigned char	*data = salt + SALT_SIZE;
	int		datalen = *buf_len - SALT_SIZE;
	unsigned char	outbuf[FRAME_SIZE_MAX];
	int		outbuf_len;
	int		err = -1;

	if (!cipher_to_nss[instance->crypto_cipher_type]) {
		return 0;
	}

	/* Create cipher context for decryption */
	decrypt_param.type = siBuffer;
	decrypt_param.data = salt;
	decrypt_param.len = SALT_SIZE;

	decrypt_context = PK11_CreateContextBySymKey(cipher_to_nss[instance->crypto_cipher_type],
						     CKA_DECRYPT,
						     instance->nss_sym_key, &decrypt_param);
	if (!decrypt_context) {
		log_printf(instance->log_level_security,
			   "PK11_CreateContext (decrypt) failed (err %d)",
			   PR_GetError());
		goto out;
	}

	if (PK11_CipherOp(decrypt_context, outbuf, &tmp1_outlen,
			  sizeof(outbuf), data, datalen) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_CipherOp (decrypt) failed (err %d)",
			   PR_GetError());
		goto out;
	}

	if (PK11_DigestFinal(decrypt_context, outbuf + tmp1_outlen, &tmp2_outlen,
			     sizeof(outbuf) - tmp1_outlen) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_DigestFinal (decrypt) failed (err %d)",
			   PR_GetError()); 
		goto out;
	}

	outbuf_len = tmp1_outlen + tmp2_outlen;

	memset(buf, 0, *buf_len);
	memcpy(buf, outbuf, outbuf_len);

	*buf_len = outbuf_len;

	err = 0;

out:
	if (decrypt_context) {
		PK11_DestroyContext(decrypt_context, PR_TRUE);
	}

	return err;
}
