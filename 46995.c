static void sctp_v4_ecn_capable(struct sock *sk)
{
	INET_ECN_xmit(sk);
}
