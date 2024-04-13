static void copy_rtnl_link_stats64(void *v, const struct rtnl_link_stats64 *b)
{
	memcpy(v, b, sizeof(*b));
}
