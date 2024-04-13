static void l2tp_ip_destroy_sock(struct sock *sk)
{
	struct sk_buff *skb;
	struct l2tp_tunnel *tunnel = l2tp_sock_to_tunnel(sk);

	while ((skb = __skb_dequeue_tail(&sk->sk_write_queue)) != NULL)
		kfree_skb(skb);

	if (tunnel) {
		l2tp_tunnel_closeall(tunnel);
		sock_put(sk);
	}

	sk_refcnt_debug_dec(sk);
}
