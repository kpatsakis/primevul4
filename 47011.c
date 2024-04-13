static void sctp_v4_to_sk_daddr(union sctp_addr *addr, struct sock *sk)
{
	inet_sk(sk)->inet_daddr = addr->v4.sin_addr.s_addr;
}
