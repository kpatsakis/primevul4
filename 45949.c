static int init_nss_db(struct crypto_instance *instance)
{
	if ((!cipher_to_nss[instance->crypto_cipher_type]) &&
	    (!hash_to_nss[instance->crypto_hash_type])) {
		return 0;
	}

	if (NSS_NoDB_Init(".") != SECSuccess) {
		log_printf(instance->log_level_security, "NSS DB initialization failed (err %d)",
			   PR_GetError());
		return -1;
	}

	return 0;
}
