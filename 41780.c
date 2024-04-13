static int xfrm_send_migrate(const struct xfrm_selector *sel, u8 dir, u8 type,
			     const struct xfrm_migrate *m, int num_migrate,
			     const struct xfrm_kmaddress *k)
{
	struct net *net = &init_net;
	struct sk_buff *skb;

	skb = nlmsg_new(xfrm_migrate_msgsize(num_migrate, !!k), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	/* build migrate */
	if (build_migrate(skb, m, num_migrate, k, sel, dir, type) < 0)
		BUG();

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_MIGRATE, GFP_ATOMIC);
}
