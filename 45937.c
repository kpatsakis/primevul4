static int authenticate_and_decrypt_nss_2_0 (
	struct crypto_instance *instance,
	unsigned char *buf,
	int *buf_len)
{
	if (hash_to_nss[instance->crypto_hash_type]) {
		unsigned char	tmp_hash[hash_len[instance->crypto_hash_type]];
		unsigned char	*hash = buf;
		unsigned char	*data = hash + hash_len[instance->crypto_hash_type];
		int		datalen = *buf_len - hash_len[instance->crypto_hash_type];

		if (calculate_nss_hash(instance, data, datalen, tmp_hash) < 0) {
			return -1;
		}

		if (memcmp(tmp_hash, hash, hash_len[instance->crypto_hash_type]) != 0) {
			log_printf(instance->log_level_error, "Digest does not match");
			return -1;
		}

		memmove(buf, data, datalen);
		*buf_len = datalen;
	}

	if (decrypt_nss(instance, buf, buf_len) < 0) {
		return -1;
	}

	return 0;
}
