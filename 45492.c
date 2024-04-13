static void ipx_destroy_socket(struct sock *sk)
{
	ipx_remove_socket(sk);
	skb_queue_purge(&sk->sk_receive_queue);
	sk_refcnt_debug_dec(sk);
}
