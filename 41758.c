static int xfrm_exp_state_notify(struct xfrm_state *x, const struct km_event *c)
{
	struct net *net = xs_net(x);
	struct sk_buff *skb;

	skb = nlmsg_new(xfrm_expire_msgsize(), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	if (build_expire(skb, x, c) < 0) {
		kfree_skb(skb);
		return -EMSGSIZE;
	}

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_EXPIRE, GFP_ATOMIC);
}
