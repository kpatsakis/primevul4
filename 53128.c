static inline bool tcp_checksum_complete_user(struct sock *sk,
					     struct sk_buff *skb)
{
	return !skb_csum_unnecessary(skb) &&
	       __tcp_checksum_complete_user(sk, skb);
}
