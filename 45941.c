int crypto_encrypt_and_sign (
	struct crypto_instance *instance,
	const unsigned char *buf_in,
	const size_t buf_in_len,
	unsigned char *buf_out,
	size_t *buf_out_len)
{
	struct crypto_config_header *cch = (struct crypto_config_header *)buf_out;
	int err;

	switch (instance->crypto_compat_type) {
		case CRYPTO_COMPAT_2_0:
			cch->crypto_cipher_type = instance->crypto_cipher_type;
			cch->crypto_hash_type = instance->crypto_hash_type;
			cch->__pad0 = 0;
			cch->__pad1 = 0;

			buf_out += sizeof(struct crypto_config_header);

			err = encrypt_and_sign_nss_2_0(instance,
						       buf_in, buf_in_len,
						       buf_out, buf_out_len);

			*buf_out_len = *buf_out_len + sizeof(struct crypto_config_header);
			break;
		case CRYPTO_COMPAT_2_2:
			cch->crypto_cipher_type = CRYPTO_CIPHER_TYPE_2_2;
			cch->crypto_hash_type = CRYPTO_HASH_TYPE_2_2;
			cch->__pad0 = 0;
			cch->__pad1 = 0;

			err = encrypt_and_sign_nss_2_2(instance,
						   buf_in, buf_in_len,
						   buf_out, buf_out_len);
			break;
		default:
			err = -1;
			break;
	}

	return err;
}
