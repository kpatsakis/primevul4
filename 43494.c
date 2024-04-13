static int curl_debug_cb(__maybe_unused CURL *handle, curl_infotype type,
			 __maybe_unused char *data, size_t size, void *userdata)
{
	struct pool *pool = (struct pool *)userdata;

	switch(type) {
		case CURLINFO_HEADER_IN:
		case CURLINFO_DATA_IN:
		case CURLINFO_SSL_DATA_IN:
			pool->sgminer_pool_stats.net_bytes_received += size;
			break;
		case CURLINFO_HEADER_OUT:
		case CURLINFO_DATA_OUT:
		case CURLINFO_SSL_DATA_OUT:
			pool->sgminer_pool_stats.net_bytes_sent += size;
			break;
		case CURLINFO_TEXT:
		default:
			break;
	}
	return 0;
}
