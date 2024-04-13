static int xfrm_get_sa(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_usersa_id *p = nlmsg_data(nlh);
	struct xfrm_state *x;
	struct sk_buff *resp_skb;
	int err = -ESRCH;

	x = xfrm_user_state_lookup(net, p, attrs, &err);
	if (x == NULL)
		goto out_noput;

	resp_skb = xfrm_state_netlink(skb, x, nlh->nlmsg_seq);
	if (IS_ERR(resp_skb)) {
		err = PTR_ERR(resp_skb);
	} else {
		err = nlmsg_unicast(net->xfrm.nlsk, resp_skb, NETLINK_CB(skb).pid);
	}
	xfrm_state_put(x);
out_noput:
	return err;
}
