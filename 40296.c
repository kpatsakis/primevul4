static int rtnl_dump_ifinfo(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	int h, s_h;
	int idx = 0, s_idx;
	struct net_device *dev;
	struct hlist_head *head;
	struct nlattr *tb[IFLA_MAX+1];
	u32 ext_filter_mask = 0;

	s_h = cb->args[0];
	s_idx = cb->args[1];

	rcu_read_lock();
	cb->seq = net->dev_base_seq;

	if (nlmsg_parse(cb->nlh, sizeof(struct rtgenmsg), tb, IFLA_MAX,
			ifla_policy) >= 0) {

		if (tb[IFLA_EXT_MASK])
			ext_filter_mask = nla_get_u32(tb[IFLA_EXT_MASK]);
	}

	for (h = s_h; h < NETDEV_HASHENTRIES; h++, s_idx = 0) {
		idx = 0;
		head = &net->dev_index_head[h];
		hlist_for_each_entry_rcu(dev, head, index_hlist) {
			if (idx < s_idx)
				goto cont;
			if (rtnl_fill_ifinfo(skb, dev, RTM_NEWLINK,
					     NETLINK_CB(cb->skb).portid,
					     cb->nlh->nlmsg_seq, 0,
					     NLM_F_MULTI,
					     ext_filter_mask) <= 0)
				goto out;

			nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
			idx++;
		}
	}
out:
	rcu_read_unlock();
	cb->args[1] = idx;
	cb->args[0] = h;

	return skb->len;
}
