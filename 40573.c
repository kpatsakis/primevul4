static int pfkey_can_dump(const struct sock *sk)
{
	if (3 * atomic_read(&sk->sk_rmem_alloc) <= 2 * sk->sk_rcvbuf)
		return 1;
	return 0;
}
