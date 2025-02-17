static int rtnetlink_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	rtnl_doit_func doit;
	int sz_idx, kind;
	int min_len;
	int family;
	int type;
	int err;

	type = nlh->nlmsg_type;
	if (type > RTM_MAX)
		return -EOPNOTSUPP;

	type -= RTM_BASE;

	/* All the messages must have at least 1 byte length */
	if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(struct rtgenmsg)))
		return 0;

	family = ((struct rtgenmsg *)NLMSG_DATA(nlh))->rtgen_family;
	sz_idx = type>>2;
	kind = type&3;

	if (kind != 2 && !ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	if (kind == 2 && nlh->nlmsg_flags&NLM_F_DUMP) {
		struct sock *rtnl;
		rtnl_dumpit_func dumpit;
		rtnl_calcit_func calcit;
		u16 min_dump_alloc = 0;

		dumpit = rtnl_get_dumpit(family, type);
		if (dumpit == NULL)
			return -EOPNOTSUPP;
		calcit = rtnl_get_calcit(family, type);
		if (calcit)
			min_dump_alloc = calcit(skb, nlh);

		__rtnl_unlock();
		rtnl = net->rtnl;
		{
			struct netlink_dump_control c = {
				.dump		= dumpit,
				.min_dump_alloc	= min_dump_alloc,
			};
			err = netlink_dump_start(rtnl, skb, nlh, &c);
		}
		rtnl_lock();
		return err;
	}

	memset(rta_buf, 0, (rtattr_max * sizeof(struct rtattr *)));

	min_len = rtm_min[sz_idx];
	if (nlh->nlmsg_len < min_len)
		return -EINVAL;

	if (nlh->nlmsg_len > min_len) {
		int attrlen = nlh->nlmsg_len - NLMSG_ALIGN(min_len);
		struct rtattr *attr = (void *)nlh + NLMSG_ALIGN(min_len);

		while (RTA_OK(attr, attrlen)) {
			unsigned int flavor = attr->rta_type;
			if (flavor) {
				if (flavor > rta_max[sz_idx])
					return -EINVAL;
				rta_buf[flavor-1] = attr;
			}
			attr = RTA_NEXT(attr, attrlen);
		}
	}

	doit = rtnl_get_doit(family, type);
	if (doit == NULL)
		return -EOPNOTSUPP;

	return doit(skb, nlh, (void *)&rta_buf[0]);
}
