static void __suspend_stratum(struct pool *pool)
{
	clear_sockbuf(pool);
	pool->stratum_active = pool->stratum_notify = false;
	if (pool->sock)
		CLOSESOCKET(pool->sock);
	pool->sock = 0;
}
