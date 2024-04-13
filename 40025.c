static void rfcomm_sock_close(struct sock *sk)
{
	lock_sock(sk);
	__rfcomm_sock_close(sk);
	release_sock(sk);
}
