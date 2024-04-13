static size_t upload_data_cb(void *ptr, size_t size, size_t nmemb,
			     void *user_data)
{
	struct upload_buffer *ub = (struct upload_buffer *)user_data;
	unsigned int len = size * nmemb;

	if (len > ub->len)
		len = ub->len;

	if (len) {
		memcpy(ptr, ub->buf, len);
		ub->buf = (uint8_t*)ub->buf + len;
		ub->len -= len;
	}

	return len;
}
