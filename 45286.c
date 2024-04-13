static int compat_rawv6_setsockopt(struct sock *sk, int level, int optname,
				   char __user *optval, unsigned int optlen)
{
	switch (level) {
	case SOL_RAW:
		break;
	case SOL_ICMPV6:
		if (inet_sk(sk)->inet_num != IPPROTO_ICMPV6)
			return -EOPNOTSUPP;
		return rawv6_seticmpfilter(sk, level, optname, optval, optlen);
	case SOL_IPV6:
		if (optname == IPV6_CHECKSUM)
			break;
	default:
		return compat_ipv6_setsockopt(sk, level, optname,
					      optval, optlen);
	}
	return do_rawv6_setsockopt(sk, level, optname, optval, optlen);
}
