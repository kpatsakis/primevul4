static void udp_sk_rx_dst_set(struct sock *sk, const struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);

	dst_hold(dst);
	sk->sk_rx_dst = dst;
}
