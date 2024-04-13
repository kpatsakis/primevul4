static struct sock *x25_alloc_socket(struct net *net)
{
	struct x25_sock *x25;
	struct sock *sk = sk_alloc(net, AF_X25, GFP_ATOMIC, &x25_proto);

	if (!sk)
		goto out;

	sock_init_data(NULL, sk);

	x25 = x25_sk(sk);
	skb_queue_head_init(&x25->ack_queue);
	skb_queue_head_init(&x25->fragment_queue);
	skb_queue_head_init(&x25->interrupt_in_queue);
	skb_queue_head_init(&x25->interrupt_out_queue);
out:
	return sk;
}
