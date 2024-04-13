static u16 rtnl_calcit(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	struct net_device *dev;
	struct nlattr *tb[IFLA_MAX+1];
	u32 ext_filter_mask = 0;
	u16 min_ifinfo_dump_size = 0;

	if (nlmsg_parse(nlh, sizeof(struct rtgenmsg), tb, IFLA_MAX,
			ifla_policy) >= 0) {
		if (tb[IFLA_EXT_MASK])
			ext_filter_mask = nla_get_u32(tb[IFLA_EXT_MASK]);
	}

	if (!ext_filter_mask)
		return NLMSG_GOODSIZE;
	/*
	 * traverse the list of net devices and compute the minimum
	 * buffer size based upon the filter mask.
	 */
	list_for_each_entry(dev, &net->dev_base_head, dev_list) {
		min_ifinfo_dump_size = max_t(u16, min_ifinfo_dump_size,
					     if_nlmsg_size(dev,
						           ext_filter_mask));
	}

	return min_ifinfo_dump_size;
}
