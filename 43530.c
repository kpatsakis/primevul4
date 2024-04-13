static bool socket_full(struct pool *pool, int wait)
{
	SOCKETTYPE sock = pool->sock;
	struct timeval timeout;
	fd_set rd;

	if (unlikely(wait < 0))
		wait = 0;
	FD_ZERO(&rd);
	FD_SET(sock, &rd);
	timeout.tv_usec = 0;
	timeout.tv_sec = wait;
	if (select(sock + 1, &rd, NULL, NULL, &timeout) > 0)
		return true;
	return false;
}
