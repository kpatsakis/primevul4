static void rawv6_close(struct sock *sk, long timeout)
{
	if (inet_sk(sk)->inet_num == IPPROTO_RAW)
		ip6_ra_control(sk, -1);
	ip6mr_sk_done(sk);
	sk_common_release(sk);
}
