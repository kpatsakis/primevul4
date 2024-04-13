static int init_nss_crypto(struct crypto_instance *instance)
{
	PK11SlotInfo*	crypt_slot = NULL;
	SECItem		crypt_param;

	if (!cipher_to_nss[instance->crypto_cipher_type]) {
		return 0;
	}

	crypt_param.type = siBuffer;
	crypt_param.data = instance->private_key;
	crypt_param.len = cipher_key_len[instance->crypto_cipher_type];

	crypt_slot = PK11_GetBestSlot(cipher_to_nss[instance->crypto_cipher_type], NULL);
	if (crypt_slot == NULL) {
		log_printf(instance->log_level_security, "Unable to find security slot (err %d)",
			   PR_GetError());
		return -1;
	}

	instance->nss_sym_key = PK11_ImportSymKey(crypt_slot,
						  cipher_to_nss[instance->crypto_cipher_type],
						  PK11_OriginUnwrap, CKA_ENCRYPT|CKA_DECRYPT,
						  &crypt_param, NULL);
	if (instance->nss_sym_key == NULL) {
		log_printf(instance->log_level_security, "Failure to import key into NSS (err %d)",
			   PR_GetError());
		return -1;
	}

	PK11_FreeSlot(crypt_slot);

	return 0;
}
