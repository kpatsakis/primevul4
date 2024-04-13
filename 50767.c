static void tcp_v6_destroy_sock(struct sock *sk)
{
	tcp_v4_destroy_sock(sk);
	inet6_destroy_sock(sk);
}
