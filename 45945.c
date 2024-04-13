static int encrypt_and_sign_nss_2_2 (
	struct crypto_instance *instance,
	const unsigned char *buf_in,
	const size_t buf_in_len,
	unsigned char *buf_out,
	size_t *buf_out_len)
{
	if (encrypt_nss(instance,
			buf_in, buf_in_len,
			buf_out + sizeof(struct crypto_config_header), buf_out_len) < 0) {
		return -1;
	}

	*buf_out_len += sizeof(struct crypto_config_header);

	if (hash_to_nss[instance->crypto_hash_type]) {
		if (calculate_nss_hash(instance, buf_out, *buf_out_len, buf_out + *buf_out_len) < 0) {
			return -1;
		}
		*buf_out_len += hash_len[instance->crypto_hash_type];
	}

	return 0;
}
