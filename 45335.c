static void pn_destruct(struct sock *sk)
{
	skb_queue_purge(&sk->sk_receive_queue);
}
