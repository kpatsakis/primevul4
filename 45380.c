static int pppoe_rcv(struct sk_buff *skb, struct net_device *dev,
		     struct packet_type *pt, struct net_device *orig_dev)
{
	struct pppoe_hdr *ph;
	struct pppox_sock *po;
	struct pppoe_net *pn;
	int len;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		goto out;

	if (!pskb_may_pull(skb, sizeof(struct pppoe_hdr)))
		goto drop;

	ph = pppoe_hdr(skb);
	len = ntohs(ph->length);

	skb_pull_rcsum(skb, sizeof(*ph));
	if (skb->len < len)
		goto drop;

	if (pskb_trim_rcsum(skb, len))
		goto drop;

	pn = pppoe_pernet(dev_net(dev));

	/* Note that get_item does a sock_hold(), so sk_pppox(po)
	 * is known to be safe.
	 */
	po = get_item(pn, ph->sid, eth_hdr(skb)->h_source, dev->ifindex);
	if (!po)
		goto drop;

	return sk_receive_skb(sk_pppox(po), skb, 0);

drop:
	kfree_skb(skb);
out:
	return NET_RX_DROP;
}
