static void ndisc_redirect_rcv(struct sk_buff *skb)
{
	u8 *hdr;
	struct ndisc_options ndopts;
	struct rd_msg *msg = (struct rd_msg *)skb_transport_header(skb);
	u32 ndoptlen = skb_tail_pointer(skb) - (skb_transport_header(skb) +
				    offsetof(struct rd_msg, opt));

#ifdef CONFIG_IPV6_NDISC_NODETYPE
	switch (skb->ndisc_nodetype) {
	case NDISC_NODETYPE_HOST:
	case NDISC_NODETYPE_NODEFAULT:
		ND_PRINTK(2, warn,
			  "Redirect: from host or unauthorized router\n");
		return;
	}
#endif

	if (!(ipv6_addr_type(&ipv6_hdr(skb)->saddr) & IPV6_ADDR_LINKLOCAL)) {
		ND_PRINTK(2, warn,
			  "Redirect: source address is not link-local\n");
		return;
	}

	if (!ndisc_parse_options(msg->opt, ndoptlen, &ndopts))
		return;

	if (!ndopts.nd_opts_rh) {
		ip6_redirect_no_header(skb, dev_net(skb->dev),
					skb->dev->ifindex, 0);
		return;
	}

	hdr = (u8 *)ndopts.nd_opts_rh;
	hdr += 8;
	if (!pskb_pull(skb, hdr - skb_transport_header(skb)))
		return;

	icmpv6_notify(skb, NDISC_REDIRECT, 0, 0);
}
