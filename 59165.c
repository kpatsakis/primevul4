static void dccp_enqueue_skb(struct sock *sk, struct sk_buff *skb)
{
	__skb_pull(skb, dccp_hdr(skb)->dccph_doff * 4);
	__skb_queue_tail(&sk->sk_receive_queue, skb);
	skb_set_owner_r(skb, sk);
	sk->sk_data_ready(sk);
}
