static void clear_sock(struct pool *pool)
{
	ssize_t n;

	mutex_lock(&pool->stratum_lock);
	do {
		if (pool->sock)
			n = recv(pool->sock, pool->sockbuf, RECVSIZE, 0);
		else
			n = 0;
	} while (n > 0);
	mutex_unlock(&pool->stratum_lock);

	clear_sockbuf(pool);
}
