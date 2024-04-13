int ndisc_rcv(struct sk_buff *skb)
{
	struct nd_msg *msg;

	if (ndisc_suppress_frag_ndisc(skb))
		return 0;

	if (skb_linearize(skb))
		return 0;

	msg = (struct nd_msg *)skb_transport_header(skb);

	__skb_push(skb, skb->data - skb_transport_header(skb));

	if (ipv6_hdr(skb)->hop_limit != 255) {
		ND_PRINTK(2, warn, "NDISC: invalid hop-limit: %d\n",
			  ipv6_hdr(skb)->hop_limit);
		return 0;
	}

	if (msg->icmph.icmp6_code != 0) {
		ND_PRINTK(2, warn, "NDISC: invalid ICMPv6 code: %d\n",
			  msg->icmph.icmp6_code);
		return 0;
	}

	memset(NEIGH_CB(skb), 0, sizeof(struct neighbour_cb));

	switch (msg->icmph.icmp6_type) {
	case NDISC_NEIGHBOUR_SOLICITATION:
		ndisc_recv_ns(skb);
		break;

	case NDISC_NEIGHBOUR_ADVERTISEMENT:
		ndisc_recv_na(skb);
		break;

	case NDISC_ROUTER_SOLICITATION:
		ndisc_recv_rs(skb);
		break;

	case NDISC_ROUTER_ADVERTISEMENT:
		ndisc_router_discovery(skb);
		break;

	case NDISC_REDIRECT:
		ndisc_redirect_rcv(skb);
		break;
	}

	return 0;
}
