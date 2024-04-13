static int xfrm_send_mapping(struct xfrm_state *x, xfrm_address_t *ipaddr,
			     __be16 sport)
{
	struct net *net = xs_net(x);
	struct sk_buff *skb;

	if (x->id.proto != IPPROTO_ESP)
		return -EINVAL;

	if (!x->encap)
		return -EINVAL;

	skb = nlmsg_new(xfrm_mapping_msgsize(), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	if (build_mapping(skb, x, ipaddr, sport) < 0)
		BUG();

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_MAPPING, GFP_ATOMIC);
}
