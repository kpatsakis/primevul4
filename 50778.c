static struct tcp_md5sig_key *tcp_v6_md5_lookup(const struct sock *sk,
						const struct sock *addr_sk)
{
	return tcp_v6_md5_do_lookup(sk, &addr_sk->sk_v6_daddr);
}
