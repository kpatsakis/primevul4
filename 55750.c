struct tcp_md5sig_pool *tcp_get_md5sig_pool(void)
{
	local_bh_disable();

	if (tcp_md5sig_pool_populated) {
		/* coupled with smp_wmb() in __tcp_alloc_md5sig_pool() */
		smp_rmb();
		return this_cpu_ptr(&tcp_md5sig_pool);
	}
	local_bh_enable();
	return NULL;
}
