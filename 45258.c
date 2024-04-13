static inline struct dgram_sock *dgram_sk(const struct sock *sk)
{
	return container_of(sk, struct dgram_sock, sk);
}
