static int pn_init(struct sock *sk)
{
	sk->sk_destruct = pn_destruct;
	return 0;
}
