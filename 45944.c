static int decrypt_nss_2_2 (
	struct crypto_instance *instance,
	unsigned char *buf,
	int *buf_len)
{
	*buf_len -= sizeof(struct crypto_config_header);

	if (decrypt_nss(instance, buf + sizeof(struct crypto_config_header), buf_len) < 0) {
		return -1;
	}

	return 0;
}
