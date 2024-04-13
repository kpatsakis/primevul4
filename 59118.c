void ip_icmp_error(struct sock *sk, struct sk_buff *skb, int err,
		   __be16 port, u32 info, u8 *payload)
{
	struct sock_exterr_skb *serr;

	skb = skb_clone(skb, GFP_ATOMIC);
	if (!skb)
		return;

	serr = SKB_EXT_ERR(skb);
	serr->ee.ee_errno = err;
	serr->ee.ee_origin = SO_EE_ORIGIN_ICMP;
	serr->ee.ee_type = icmp_hdr(skb)->type;
	serr->ee.ee_code = icmp_hdr(skb)->code;
	serr->ee.ee_pad = 0;
	serr->ee.ee_info = info;
	serr->ee.ee_data = 0;
	serr->addr_offset = (u8 *)&(((struct iphdr *)(icmp_hdr(skb) + 1))->daddr) -
				   skb_network_header(skb);
	serr->port = port;

	if (skb_pull(skb, payload - skb->data)) {
		skb_reset_transport_header(skb);
		if (sock_queue_err_skb(sk, skb) == 0)
			return;
	}
	kfree_skb(skb);
}
