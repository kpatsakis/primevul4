int icmpv6_push_pending_frames(struct sock *sk, struct flowi6 *fl6,
			       struct icmp6hdr *thdr, int len)
{
	struct sk_buff *skb;
	struct icmp6hdr *icmp6h;
	int err = 0;

	skb = skb_peek(&sk->sk_write_queue);
	if (!skb)
		goto out;

	icmp6h = icmp6_hdr(skb);
	memcpy(icmp6h, thdr, sizeof(struct icmp6hdr));
	icmp6h->icmp6_cksum = 0;

	if (skb_queue_len(&sk->sk_write_queue) == 1) {
		skb->csum = csum_partial(icmp6h,
					sizeof(struct icmp6hdr), skb->csum);
		icmp6h->icmp6_cksum = csum_ipv6_magic(&fl6->saddr,
						      &fl6->daddr,
						      len, fl6->flowi6_proto,
						      skb->csum);
	} else {
		__wsum tmp_csum = 0;

		skb_queue_walk(&sk->sk_write_queue, skb) {
			tmp_csum = csum_add(tmp_csum, skb->csum);
		}

		tmp_csum = csum_partial(icmp6h,
					sizeof(struct icmp6hdr), tmp_csum);
		icmp6h->icmp6_cksum = csum_ipv6_magic(&fl6->saddr,
						      &fl6->daddr,
						      len, fl6->flowi6_proto,
						      tmp_csum);
	}
	ip6_push_pending_frames(sk);
out:
	return err;
}
