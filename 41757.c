static int xfrm_exp_policy_notify(struct xfrm_policy *xp, int dir, const struct km_event *c)
{
	struct net *net = xp_net(xp);
	struct sk_buff *skb;

	skb = nlmsg_new(xfrm_polexpire_msgsize(xp), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	if (build_polexpire(skb, xp, dir, c) < 0)
		BUG();

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_EXPIRE, GFP_ATOMIC);
}
