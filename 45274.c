struct sk_buff *skb_udp_tunnel_segment(struct sk_buff *skb,
				       netdev_features_t features)
{
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	int mac_len = skb->mac_len;
	int tnl_hlen = skb_inner_mac_header(skb) - skb_transport_header(skb);
	__be16 protocol = skb->protocol;
	netdev_features_t enc_features;
	int outer_hlen;

	if (unlikely(!pskb_may_pull(skb, tnl_hlen)))
		goto out;

	skb->encapsulation = 0;
	__skb_pull(skb, tnl_hlen);
	skb_reset_mac_header(skb);
	skb_set_network_header(skb, skb_inner_network_offset(skb));
	skb->mac_len = skb_inner_network_offset(skb);
	skb->protocol = htons(ETH_P_TEB);

	/* segment inner packet. */
	enc_features = skb->dev->hw_enc_features & netif_skb_features(skb);
	segs = skb_mac_gso_segment(skb, enc_features);
	if (!segs || IS_ERR(segs))
		goto out;

	outer_hlen = skb_tnl_header_len(skb);
	skb = segs;
	do {
		struct udphdr *uh;
		int udp_offset = outer_hlen - tnl_hlen;

		skb_reset_inner_headers(skb);
		skb->encapsulation = 1;

		skb->mac_len = mac_len;

		skb_push(skb, outer_hlen);
		skb_reset_mac_header(skb);
		skb_set_network_header(skb, mac_len);
		skb_set_transport_header(skb, udp_offset);
		uh = udp_hdr(skb);
		uh->len = htons(skb->len - udp_offset);

		/* csum segment if tunnel sets skb with csum. */
		if (protocol == htons(ETH_P_IP) && unlikely(uh->check)) {
			struct iphdr *iph = ip_hdr(skb);

			uh->check = ~csum_tcpudp_magic(iph->saddr, iph->daddr,
						       skb->len - udp_offset,
						       IPPROTO_UDP, 0);
			uh->check = csum_fold(skb_checksum(skb, udp_offset,
							   skb->len - udp_offset, 0));
			if (uh->check == 0)
				uh->check = CSUM_MANGLED_0;

		} else if (protocol == htons(ETH_P_IPV6)) {
			struct ipv6hdr *ipv6h = ipv6_hdr(skb);
			u32 len = skb->len - udp_offset;

			uh->check = ~csum_ipv6_magic(&ipv6h->saddr, &ipv6h->daddr,
						     len, IPPROTO_UDP, 0);
			uh->check = csum_fold(skb_checksum(skb, udp_offset, len, 0));
			if (uh->check == 0)
				uh->check = CSUM_MANGLED_0;
			skb->ip_summed = CHECKSUM_NONE;
		}

		skb->protocol = protocol;
	} while ((skb = skb->next));
out:
	return segs;
}
