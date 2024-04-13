bool tcp_alloc_md5sig_pool(void)
{
	if (unlikely(!tcp_md5sig_pool_populated)) {
		mutex_lock(&tcp_md5sig_mutex);

		if (!tcp_md5sig_pool_populated)
			__tcp_alloc_md5sig_pool();

		mutex_unlock(&tcp_md5sig_mutex);
	}
	return tcp_md5sig_pool_populated;
}
