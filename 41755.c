static int xfrm_dump_sa(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state_walk *walk = (struct xfrm_state_walk *) &cb->args[1];
	struct xfrm_dump_info info;

	BUILD_BUG_ON(sizeof(struct xfrm_state_walk) >
		     sizeof(cb->args) - sizeof(cb->args[0]));

	info.in_skb = cb->skb;
	info.out_skb = skb;
	info.nlmsg_seq = cb->nlh->nlmsg_seq;
	info.nlmsg_flags = NLM_F_MULTI;

	if (!cb->args[0]) {
		cb->args[0] = 1;
		xfrm_state_walk_init(walk, 0);
	}

	(void) xfrm_state_walk(net, walk, dump_one_state, &info);

	return skb->len;
}
