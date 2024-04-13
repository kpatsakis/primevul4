static inline struct pfkey_sock *pfkey_sk(struct sock *sk)
{
	return (struct pfkey_sock *)sk;
}
