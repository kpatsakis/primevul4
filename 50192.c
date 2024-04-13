void sock_kzfree_s(struct sock *sk, void *mem, int size)
{
	__sock_kfree_s(sk, mem, size, true);
}
