static int rtnl_getlink(struct sk_buff *skb, struct nlmsghdr* nlh, void *arg)
{
	struct net *net = sock_net(skb->sk);
	struct ifinfomsg *ifm;
	char ifname[IFNAMSIZ];
	struct nlattr *tb[IFLA_MAX+1];
	struct net_device *dev = NULL;
	struct sk_buff *nskb;
	int err;
	u32 ext_filter_mask = 0;

	err = nlmsg_parse(nlh, sizeof(*ifm), tb, IFLA_MAX, ifla_policy);
	if (err < 0)
		return err;

	if (tb[IFLA_IFNAME])
		nla_strlcpy(ifname, tb[IFLA_IFNAME], IFNAMSIZ);

	if (tb[IFLA_EXT_MASK])
		ext_filter_mask = nla_get_u32(tb[IFLA_EXT_MASK]);

	ifm = nlmsg_data(nlh);
	if (ifm->ifi_index > 0)
		dev = __dev_get_by_index(net, ifm->ifi_index);
	else if (tb[IFLA_IFNAME])
		dev = __dev_get_by_name(net, ifname);
	else
		return -EINVAL;

	if (dev == NULL)
		return -ENODEV;

	nskb = nlmsg_new(if_nlmsg_size(dev, ext_filter_mask), GFP_KERNEL);
	if (nskb == NULL)
		return -ENOBUFS;

	err = rtnl_fill_ifinfo(nskb, dev, RTM_NEWLINK, NETLINK_CB(skb).portid,
			       nlh->nlmsg_seq, 0, 0, ext_filter_mask);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in if_nlmsg_size */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(nskb);
	} else
		err = rtnl_unicast(nskb, net, NETLINK_CB(skb).portid);

	return err;
}
