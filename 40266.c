static inline size_t bridge_nlmsg_size(void)
{
	return NLMSG_ALIGN(sizeof(struct ifinfomsg))
		+ nla_total_size(IFNAMSIZ)	/* IFLA_IFNAME */
		+ nla_total_size(MAX_ADDR_LEN)	/* IFLA_ADDRESS */
		+ nla_total_size(sizeof(u32))	/* IFLA_MASTER */
		+ nla_total_size(sizeof(u32))	/* IFLA_MTU */
		+ nla_total_size(sizeof(u32))	/* IFLA_LINK */
		+ nla_total_size(sizeof(u32))	/* IFLA_OPERSTATE */
		+ nla_total_size(sizeof(u8))	/* IFLA_PROTINFO */
		+ nla_total_size(sizeof(struct nlattr))	/* IFLA_AF_SPEC */
		+ nla_total_size(sizeof(u16))	/* IFLA_BRIDGE_FLAGS */
		+ nla_total_size(sizeof(u16));	/* IFLA_BRIDGE_MODE */
}
