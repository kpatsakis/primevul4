static bool tcp_should_autocork(struct sock *sk, struct sk_buff *skb,
				int size_goal)
{
	return skb->len < size_goal &&
	       sysctl_tcp_autocorking &&
	       skb != tcp_write_queue_head(sk) &&
	       atomic_read(&sk->sk_wmem_alloc) > skb->truesize;
}
