static void pn_sock_close(struct sock *sk, long timeout)
{
	sk_common_release(sk);
}
