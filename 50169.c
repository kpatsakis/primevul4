bool sk_page_frag_refill(struct sock *sk, struct page_frag *pfrag)
{
	if (likely(skb_page_frag_refill(32U, pfrag, sk->sk_allocation)))
		return true;

	sk_enter_memory_pressure(sk);
	sk_stream_moderate_sndbuf(sk);
	return false;
}
