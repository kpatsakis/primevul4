static inline size_t rtnl_fdb_nlmsg_size(void)
{
	return NLMSG_ALIGN(sizeof(struct ndmsg)) + nla_total_size(ETH_ALEN);
}
