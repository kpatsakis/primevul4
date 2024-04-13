static int rtnl_newlink(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct net *net = sock_net(skb->sk);
	const struct rtnl_link_ops *ops;
	struct net_device *dev;
	struct ifinfomsg *ifm;
	char kind[MODULE_NAME_LEN];
	char ifname[IFNAMSIZ];
	struct nlattr *tb[IFLA_MAX+1];
	struct nlattr *linkinfo[IFLA_INFO_MAX+1];
	int err;

#ifdef CONFIG_MODULES
replay:
#endif
	err = nlmsg_parse(nlh, sizeof(*ifm), tb, IFLA_MAX, ifla_policy);
	if (err < 0)
		return err;

	if (tb[IFLA_IFNAME])
		nla_strlcpy(ifname, tb[IFLA_IFNAME], IFNAMSIZ);
	else
		ifname[0] = '\0';

	ifm = nlmsg_data(nlh);
	if (ifm->ifi_index > 0)
		dev = __dev_get_by_index(net, ifm->ifi_index);
	else {
		if (ifname[0])
			dev = __dev_get_by_name(net, ifname);
		else
			dev = NULL;
	}

	err = validate_linkmsg(dev, tb);
	if (err < 0)
		return err;

	if (tb[IFLA_LINKINFO]) {
		err = nla_parse_nested(linkinfo, IFLA_INFO_MAX,
				       tb[IFLA_LINKINFO], ifla_info_policy);
		if (err < 0)
			return err;
	} else
		memset(linkinfo, 0, sizeof(linkinfo));

	if (linkinfo[IFLA_INFO_KIND]) {
		nla_strlcpy(kind, linkinfo[IFLA_INFO_KIND], sizeof(kind));
		ops = rtnl_link_ops_get(kind);
	} else {
		kind[0] = '\0';
		ops = NULL;
	}

	if (1) {
		struct nlattr *attr[ops ? ops->maxtype + 1 : 0], **data = NULL;
		struct net *dest_net;

		if (ops) {
			if (ops->maxtype && linkinfo[IFLA_INFO_DATA]) {
				err = nla_parse_nested(attr, ops->maxtype,
						       linkinfo[IFLA_INFO_DATA],
						       ops->policy);
				if (err < 0)
					return err;
				data = attr;
			}
			if (ops->validate) {
				err = ops->validate(tb, data);
				if (err < 0)
					return err;
			}
		}

		if (dev) {
			int modified = 0;

			if (nlh->nlmsg_flags & NLM_F_EXCL)
				return -EEXIST;
			if (nlh->nlmsg_flags & NLM_F_REPLACE)
				return -EOPNOTSUPP;

			if (linkinfo[IFLA_INFO_DATA]) {
				if (!ops || ops != dev->rtnl_link_ops ||
				    !ops->changelink)
					return -EOPNOTSUPP;

				err = ops->changelink(dev, tb, data);
				if (err < 0)
					return err;
				modified = 1;
			}

			return do_setlink(dev, ifm, tb, ifname, modified);
		}

		if (!(nlh->nlmsg_flags & NLM_F_CREATE)) {
			if (ifm->ifi_index == 0 && tb[IFLA_GROUP])
				return rtnl_group_changelink(net,
						nla_get_u32(tb[IFLA_GROUP]),
						ifm, tb);
			return -ENODEV;
		}

		if (tb[IFLA_MAP] || tb[IFLA_MASTER] || tb[IFLA_PROTINFO])
			return -EOPNOTSUPP;

		if (!ops) {
#ifdef CONFIG_MODULES
			if (kind[0]) {
				__rtnl_unlock();
				request_module("rtnl-link-%s", kind);
				rtnl_lock();
				ops = rtnl_link_ops_get(kind);
				if (ops)
					goto replay;
			}
#endif
			return -EOPNOTSUPP;
		}

		if (!ifname[0])
			snprintf(ifname, IFNAMSIZ, "%s%%d", ops->kind);

		dest_net = rtnl_link_get_net(net, tb);
		if (IS_ERR(dest_net))
			return PTR_ERR(dest_net);

		dev = rtnl_create_link(dest_net, ifname, ops, tb);
		if (IS_ERR(dev)) {
			err = PTR_ERR(dev);
			goto out;
		}

		dev->ifindex = ifm->ifi_index;

		if (ops->newlink)
			err = ops->newlink(net, dev, tb, data);
		else
			err = register_netdevice(dev);

		if (err < 0 && !IS_ERR(dev))
			free_netdev(dev);
		if (err < 0)
			goto out;

		err = rtnl_configure_link(dev, ifm);
		if (err < 0)
			unregister_netdevice(dev);
out:
		put_net(dest_net);
		return err;
	}
}
