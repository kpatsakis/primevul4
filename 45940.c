int crypto_authenticate_and_decrypt (struct crypto_instance *instance,
	unsigned char *buf,
	int *buf_len)
{
	struct crypto_config_header *cch = (struct crypto_config_header *)buf;

	switch (instance->crypto_compat_type) {
		case CRYPTO_COMPAT_2_0:

			/*
			 * decode crypto config of incoming packets
			 */

			if (cch->crypto_cipher_type != instance->crypto_cipher_type) {
				log_printf(instance->log_level_security,
					   "Incoming packet has different crypto type. Rejecting");
				return -1;
			}

			if (cch->crypto_hash_type != instance->crypto_hash_type) {
				log_printf(instance->log_level_security,
					   "Incoming packet has different hash type. Rejecting");
				return -1;
			}

			if ((cch->__pad0 != 0) || (cch->__pad1 != 0)) {
				log_printf(instance->log_level_security,
					   "Incoming packet appears to have features not supported by this version of corosync. Rejecting");
				return -1;
			}

			/*
			 * invalidate config header and kill it
			 */

			cch = NULL;
			*buf_len -= sizeof(struct crypto_config_header);
			memmove(buf, buf + sizeof(struct crypto_config_header), *buf_len);

			return authenticate_and_decrypt_nss_2_0(instance, buf, buf_len);
			break;
		case CRYPTO_COMPAT_2_2:
			if (cch->crypto_cipher_type != CRYPTO_CIPHER_TYPE_2_2) {
				log_printf(instance->log_level_security,
					   "Incoming packet has different crypto type. Rejecting");
				return -1;
			}

			if (cch->crypto_hash_type != CRYPTO_HASH_TYPE_2_2) {
				log_printf(instance->log_level_security,
					   "Incoming packet has different hash type. Rejecting");
				return -1;
			}

			/*
			 * authenticate packet first
			 */

			if (authenticate_nss_2_2(instance, buf, buf_len) != 0) {
				return -1;
			}

			/*
			 * now we can "trust" the padding bytes/future features
			 */

			if ((cch->__pad0 != 0) || (cch->__pad1 != 0)) {
				log_printf(instance->log_level_security,
					   "Incoming packet appears to have features not supported by this version of corosync. Rejecting");
				return -1;
			}

			/*
			 * decrypt
			 */

			if (decrypt_nss_2_2(instance, buf, buf_len) != 0) {
				return -1;
			}

			/*
			 * invalidate config header and kill it
			 */
			cch = NULL;
			memmove(buf, buf + sizeof(struct crypto_config_header), *buf_len);

			return 0;
			break;
		default:
			return -1;
			break;
	}
}
