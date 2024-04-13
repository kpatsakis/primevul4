static void clear_sockbuf(struct pool *pool)
{
	strcpy(pool->sockbuf, "");
}
