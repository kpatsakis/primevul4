static int rawv6_init_sk(struct sock *sk)
{
	struct raw6_sock *rp = raw6_sk(sk);

	switch (inet_sk(sk)->inet_num) {
	case IPPROTO_ICMPV6:
		rp->checksum = 1;
		rp->offset   = 2;
		break;
	case IPPROTO_MH:
		rp->checksum = 1;
		rp->offset   = 4;
		break;
	default:
		break;
	}
	return 0;
}
