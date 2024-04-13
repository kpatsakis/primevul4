static int init_nss(struct crypto_instance *instance,
		    const char *crypto_cipher_type,
		    const char *crypto_hash_type,
		    const char *crypto_compat_type)
{
	log_printf(instance->log_level_notice,
		   "Initializing transmit/receive security (NSS) crypto: %s hash: %s compat: %s",
		   crypto_cipher_type, crypto_hash_type, crypto_compat_type);

	if (init_nss_db(instance) < 0) {
		return -1;
	}

	if (init_nss_crypto(instance) < 0) {
		return -1;
	}

	if (init_nss_hash(instance) < 0) {
		return -1;
	}

	return 0;
}
