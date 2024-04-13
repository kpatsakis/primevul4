static void dgram_close(struct sock *sk, long timeout)
{
	sk_common_release(sk);
}
