static void sctp_v6_to_sk_daddr(union sctp_addr *addr, struct sock *sk)
{
	if (addr->sa.sa_family == AF_INET) {
		sk->sk_v6_daddr.s6_addr32[0] = 0;
		sk->sk_v6_daddr.s6_addr32[1] = 0;
		sk->sk_v6_daddr.s6_addr32[2] = htonl(0x0000ffff);
		sk->sk_v6_daddr.s6_addr32[3] = addr->v4.sin_addr.s_addr;
	} else {
		sk->sk_v6_daddr = addr->v6.sin6_addr;
	}
}
