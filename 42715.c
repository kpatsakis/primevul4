static int sctp_writeable(struct sock *sk)
{
	int amt = 0;

	amt = sk->sk_sndbuf - atomic_read(&sk->sk_wmem_alloc);
	if (amt < 0)
		amt = 0;
	return amt;
}
