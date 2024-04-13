static int crypto_user_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct nlattr *attrs[CRYPTOCFGA_MAX+1];
	struct crypto_link *link;
	int type, err;

	type = nlh->nlmsg_type;
	if (type > CRYPTO_MSG_MAX)
		return -EINVAL;

	type -= CRYPTO_MSG_BASE;
	link = &crypto_dispatch[type];

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if ((type == (CRYPTO_MSG_GETALG - CRYPTO_MSG_BASE) &&
	    (nlh->nlmsg_flags & NLM_F_DUMP))) {
		struct crypto_alg *alg;
		u16 dump_alloc = 0;

		if (link->dump == NULL)
			return -EINVAL;

		list_for_each_entry(alg, &crypto_alg_list, cra_list)
			dump_alloc += CRYPTO_REPORT_MAXSIZE;

		{
			struct netlink_dump_control c = {
				.dump = link->dump,
				.done = link->done,
				.min_dump_alloc = dump_alloc,
			};
			return netlink_dump_start(crypto_nlsk, skb, nlh, &c);
		}
	}

	err = nlmsg_parse(nlh, crypto_msg_min[type], attrs, CRYPTOCFGA_MAX,
			  crypto_policy);
	if (err < 0)
		return err;

	if (link->doit == NULL)
		return -EINVAL;

	return link->doit(skb, nlh, attrs);
}
