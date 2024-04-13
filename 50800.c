static int nfnetlink_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	const struct nfnl_callback *nc;
	const struct nfnetlink_subsystem *ss;
	int type, err;

	/* All the messages must at least contain nfgenmsg */
	if (nlmsg_len(nlh) < sizeof(struct nfgenmsg))
		return 0;

	type = nlh->nlmsg_type;
replay:
	rcu_read_lock();
	ss = nfnetlink_get_subsys(type);
	if (!ss) {
#ifdef CONFIG_MODULES
		rcu_read_unlock();
		request_module("nfnetlink-subsys-%d", NFNL_SUBSYS_ID(type));
		rcu_read_lock();
		ss = nfnetlink_get_subsys(type);
		if (!ss)
#endif
		{
			rcu_read_unlock();
			return -EINVAL;
		}
	}

	nc = nfnetlink_find_client(type, ss);
	if (!nc) {
		rcu_read_unlock();
		return -EINVAL;
	}

	{
		int min_len = nlmsg_total_size(sizeof(struct nfgenmsg));
		u_int8_t cb_id = NFNL_MSG_TYPE(nlh->nlmsg_type);
		struct nlattr *cda[ss->cb[cb_id].attr_count + 1];
		struct nlattr *attr = (void *)nlh + min_len;
		int attrlen = nlh->nlmsg_len - min_len;
		__u8 subsys_id = NFNL_SUBSYS_ID(type);

		err = nla_parse(cda, ss->cb[cb_id].attr_count,
				attr, attrlen, ss->cb[cb_id].policy);
		if (err < 0) {
			rcu_read_unlock();
			return err;
		}

		if (nc->call_rcu) {
			err = nc->call_rcu(net, net->nfnl, skb, nlh,
					   (const struct nlattr **)cda);
			rcu_read_unlock();
		} else {
			rcu_read_unlock();
			nfnl_lock(subsys_id);
			if (nfnl_dereference_protected(subsys_id) != ss ||
			    nfnetlink_find_client(type, ss) != nc)
				err = -EAGAIN;
			else if (nc->call)
				err = nc->call(net, net->nfnl, skb, nlh,
					       (const struct nlattr **)cda);
			else
				err = -EINVAL;
			nfnl_unlock(subsys_id);
		}
		if (err == -EAGAIN)
			goto replay;
		return err;
	}
}
