static int xfrm_get_spdinfo(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct sk_buff *r_skb;
	u32 *flags = nlmsg_data(nlh);
	u32 spid = NETLINK_CB(skb).pid;
	u32 seq = nlh->nlmsg_seq;

	r_skb = nlmsg_new(xfrm_spdinfo_msgsize(), GFP_ATOMIC);
	if (r_skb == NULL)
		return -ENOMEM;

	if (build_spdinfo(r_skb, net, spid, seq, *flags) < 0)
		BUG();

	return nlmsg_unicast(net->xfrm.nlsk, r_skb, spid);
}
