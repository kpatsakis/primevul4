static int l2tp_ip_backlog_recv(struct sock *sk, struct sk_buff *skb)
{
	int rc;

	/* Charge it to the socket, dropping if the queue is full. */
	rc = sock_queue_rcv_skb(sk, skb);
	if (rc < 0)
		goto drop;

	return 0;

drop:
	IP_INC_STATS(sock_net(sk), IPSTATS_MIB_INDISCARDS);
	kfree_skb(skb);
	return -1;
}
