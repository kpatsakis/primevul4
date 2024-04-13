static int xfrm_aevent_state_notify(struct xfrm_state *x, const struct km_event *c)
{
	struct net *net = xs_net(x);
	struct sk_buff *skb;

	skb = nlmsg_new(xfrm_aevent_msgsize(x), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	if (build_aevent(skb, x, c) < 0)
		BUG();

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_AEVENTS, GFP_ATOMIC);
}
