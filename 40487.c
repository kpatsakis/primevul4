static int crypto_report(struct sk_buff *in_skb, struct nlmsghdr *in_nlh,
			 struct nlattr **attrs)
{
	struct crypto_user_alg *p = nlmsg_data(in_nlh);
	struct crypto_alg *alg;
	struct sk_buff *skb;
	struct crypto_dump_info info;
	int err;

	if (!p->cru_driver_name)
		return -EINVAL;

	alg = crypto_alg_match(p, 1);
	if (!alg)
		return -ENOENT;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	info.in_skb = in_skb;
	info.out_skb = skb;
	info.nlmsg_seq = in_nlh->nlmsg_seq;
	info.nlmsg_flags = 0;

	err = crypto_report_alg(alg, &info);
	if (err)
		return err;

	return nlmsg_unicast(crypto_nlsk, skb, NETLINK_CB(in_skb).portid);
}
