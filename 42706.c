void sctp_sock_rfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	struct sctp_ulpevent *event = sctp_skb2event(skb);

	atomic_sub(event->rmem_len, &sk->sk_rmem_alloc);
}
