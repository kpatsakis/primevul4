static int dgram_getsockopt(struct sock *sk, int level, int optname,
		    char __user *optval, int __user *optlen)
{
	struct dgram_sock *ro = dgram_sk(sk);

	int val, len;

	if (level != SOL_IEEE802154)
		return -EOPNOTSUPP;

	if (get_user(len, optlen))
		return -EFAULT;

	len = min_t(unsigned int, len, sizeof(int));

	switch (optname) {
	case WPAN_WANTACK:
		val = ro->want_ack;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;
	return 0;
}
