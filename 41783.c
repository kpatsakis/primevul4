static int xfrm_send_report(struct net *net, u8 proto,
			    struct xfrm_selector *sel, xfrm_address_t *addr)
{
	struct sk_buff *skb;

	skb = nlmsg_new(xfrm_report_msgsize(), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	if (build_report(skb, proto, sel, addr) < 0)
		BUG();

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_REPORT, GFP_ATOMIC);
}
