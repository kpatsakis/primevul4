static int do_rawv6_getsockopt(struct sock *sk, int level, int optname,
			    char __user *optval, int __user *optlen)
{
	struct raw6_sock *rp = raw6_sk(sk);
	int val, len;

	if (get_user(len,optlen))
		return -EFAULT;

	switch (optname) {
	case IPV6_CHECKSUM:
		/*
		 * We allow getsockopt() for IPPROTO_IPV6-level
		 * IPV6_CHECKSUM socket option on ICMPv6 sockets
		 * since RFC3542 is silent about it.
		 */
		if (rp->checksum == 0)
			val = -1;
		else
			val = rp->offset;
		break;

	default:
		return -ENOPROTOOPT;
	}

	len = min_t(unsigned int, sizeof(int), len);

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval,&val,len))
		return -EFAULT;
	return 0;
}
