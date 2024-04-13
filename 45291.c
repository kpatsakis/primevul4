static void raw6_destroy(struct sock *sk)
{
	lock_sock(sk);
	ip6_flush_pending_frames(sk);
	release_sock(sk);

	inet6_destroy_sock(sk);
}
