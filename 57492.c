static inline int ip6_forward_finish(struct net *net, struct sock *sk,
				     struct sk_buff *skb)
{
	return dst_output(net, sk, skb);
}
