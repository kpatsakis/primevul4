static __sum16 *skb_checksum_setup_ip(struct sk_buff *skb,
				      typeof(IPPROTO_IP) proto,
				      unsigned int off)
{
	switch (proto) {
		int err;

	case IPPROTO_TCP:
		err = skb_maybe_pull_tail(skb, off + sizeof(struct tcphdr),
					  off + MAX_TCP_HDR_LEN);
		if (!err && !skb_partial_csum_set(skb, off,
						  offsetof(struct tcphdr,
							   check)))
			err = -EPROTO;
		return err ? ERR_PTR(err) : &tcp_hdr(skb)->check;

	case IPPROTO_UDP:
		err = skb_maybe_pull_tail(skb, off + sizeof(struct udphdr),
					  off + sizeof(struct udphdr));
		if (!err && !skb_partial_csum_set(skb, off,
						  offsetof(struct udphdr,
							   check)))
			err = -EPROTO;
		return err ? ERR_PTR(err) : &udp_hdr(skb)->check;
	}

	return ERR_PTR(-EPROTO);
}
