static int tipc_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	unsigned int truesize = skb->truesize;

	if (likely(filter_rcv(sk, skb)))
		atomic_add(truesize, &tipc_sk(sk)->dupl_rcvcnt);
	return 0;
}
