static void wakeupdispatch(struct tipc_port *tport)
{
	struct sock *sk = tport->sk;

	sk->sk_write_space(sk);
}
