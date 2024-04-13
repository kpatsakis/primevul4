static struct sk_buff *ndisc_alloc_skb(struct net_device *dev,
				       int len)
{
	int hlen = LL_RESERVED_SPACE(dev);
	int tlen = dev->needed_tailroom;
	struct sock *sk = dev_net(dev)->ipv6.ndisc_sk;
	struct sk_buff *skb;

	skb = alloc_skb(hlen + sizeof(struct ipv6hdr) + len + tlen, GFP_ATOMIC);
	if (!skb) {
		ND_PRINTK(0, err, "ndisc: %s failed to allocate an skb\n",
			  __func__);
		return NULL;
	}

	skb->protocol = htons(ETH_P_IPV6);
	skb->dev = dev;

	skb_reserve(skb, hlen + sizeof(struct ipv6hdr));
	skb_reset_transport_header(skb);

	/* Manually assign socket ownership as we avoid calling
	 * sock_alloc_send_pskb() to bypass wmem buffer limits
	 */
	skb_set_owner_w(skb, sk);

	return skb;
}
