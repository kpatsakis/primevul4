static void recalloc_sock(struct pool *pool, size_t len)
{
	size_t old, newlen;

	old = strlen(pool->sockbuf);
	newlen = old + len + 1;
	if (newlen < pool->sockbuf_size)
		return;
	newlen = newlen + (RBUFSIZE - (newlen % RBUFSIZE));
	pool->sockbuf = (char *)realloc(pool->sockbuf, newlen);
	if (!pool->sockbuf)
		quithere(1, "Failed to realloc pool sockbuf");
	memset(pool->sockbuf + old, 0, newlen - old);
	pool->sockbuf_size = newlen;
}
