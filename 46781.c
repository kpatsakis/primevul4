void udp4_hwcsum(struct sk_buff *skb, __be32 src, __be32 dst)
{
	struct udphdr *uh = udp_hdr(skb);
	int offset = skb_transport_offset(skb);
	int len = skb->len - offset;
	int hlen = len;
	__wsum csum = 0;

	if (!skb_has_frag_list(skb)) {
		/*
		 * Only one fragment on the socket.
		 */
		skb->csum_start = skb_transport_header(skb) - skb->head;
		skb->csum_offset = offsetof(struct udphdr, check);
		uh->check = ~csum_tcpudp_magic(src, dst, len,
					       IPPROTO_UDP, 0);
	} else {
		struct sk_buff *frags;

		/*
		 * HW-checksum won't work as there are two or more
		 * fragments on the socket so that all csums of sk_buffs
		 * should be together
		 */
		skb_walk_frags(skb, frags) {
			csum = csum_add(csum, frags->csum);
			hlen -= frags->len;
		}

		csum = skb_checksum(skb, offset, hlen, csum);
		skb->ip_summed = CHECKSUM_NONE;

		uh->check = csum_tcpudp_magic(src, dst, len, IPPROTO_UDP, csum);
		if (uh->check == 0)
			uh->check = CSUM_MANGLED_0;
	}
}
