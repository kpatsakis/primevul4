static void sctp_v4_from_sk(union sctp_addr *addr, struct sock *sk)
{
	addr->v4.sin_family = AF_INET;
	addr->v4.sin_port = 0;
	addr->v4.sin_addr.s_addr = inet_sk(sk)->inet_rcv_saddr;
}
