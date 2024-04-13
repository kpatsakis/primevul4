static void l2tp_ip6_destroy_sock(struct sock *sk)
{
	lock_sock(sk);
	ip6_flush_pending_frames(sk);
	release_sock(sk);

	inet6_destroy_sock(sk);
}
