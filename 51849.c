static struct tipc_sock *tipc_sk(const struct sock *sk)
{
	return container_of(sk, struct tipc_sock, sk);
}
