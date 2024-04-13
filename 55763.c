static void tcp_prequeue_process(struct sock *sk)
{
	struct sk_buff *skb;
	struct tcp_sock *tp = tcp_sk(sk);

	NET_INC_STATS(sock_net(sk), LINUX_MIB_TCPPREQUEUED);

	while ((skb = __skb_dequeue(&tp->ucopy.prequeue)) != NULL)
		sk_backlog_rcv(sk, skb);

	/* Clear memory counter. */
	tp->ucopy.memory = 0;
}
