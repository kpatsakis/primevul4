static int do_ip_getsockopt(struct sock *sk, int level, int optname,
			    char __user *optval, int __user *optlen, unsigned int flags)
{
	struct inet_sock *inet = inet_sk(sk);
	bool needs_rtnl = getsockopt_needs_rtnl(optname);
	int val, err = 0;
	int len;

	if (level != SOL_IP)
		return -EOPNOTSUPP;

	if (ip_mroute_opt(optname))
		return ip_mroute_getsockopt(sk, optname, optval, optlen);

	if (get_user(len, optlen))
		return -EFAULT;
	if (len < 0)
		return -EINVAL;

	if (needs_rtnl)
		rtnl_lock();
	lock_sock(sk);

	switch (optname) {
	case IP_OPTIONS:
	{
		unsigned char optbuf[sizeof(struct ip_options)+40];
		struct ip_options *opt = (struct ip_options *)optbuf;
		struct ip_options_rcu *inet_opt;

		inet_opt = rcu_dereference_protected(inet->inet_opt,
						     lockdep_sock_is_held(sk));
		opt->optlen = 0;
		if (inet_opt)
			memcpy(optbuf, &inet_opt->opt,
			       sizeof(struct ip_options) +
			       inet_opt->opt.optlen);
		release_sock(sk);

		if (opt->optlen == 0)
			return put_user(0, optlen);

		ip_options_undo(opt);

		len = min_t(unsigned int, len, opt->optlen);
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, opt->__data, len))
			return -EFAULT;
		return 0;
	}
	case IP_PKTINFO:
		val = (inet->cmsg_flags & IP_CMSG_PKTINFO) != 0;
		break;
	case IP_RECVTTL:
		val = (inet->cmsg_flags & IP_CMSG_TTL) != 0;
		break;
	case IP_RECVTOS:
		val = (inet->cmsg_flags & IP_CMSG_TOS) != 0;
		break;
	case IP_RECVOPTS:
		val = (inet->cmsg_flags & IP_CMSG_RECVOPTS) != 0;
		break;
	case IP_RETOPTS:
		val = (inet->cmsg_flags & IP_CMSG_RETOPTS) != 0;
		break;
	case IP_PASSSEC:
		val = (inet->cmsg_flags & IP_CMSG_PASSSEC) != 0;
		break;
	case IP_RECVORIGDSTADDR:
		val = (inet->cmsg_flags & IP_CMSG_ORIGDSTADDR) != 0;
		break;
	case IP_CHECKSUM:
		val = (inet->cmsg_flags & IP_CMSG_CHECKSUM) != 0;
		break;
	case IP_RECVFRAGSIZE:
		val = (inet->cmsg_flags & IP_CMSG_RECVFRAGSIZE) != 0;
		break;
	case IP_TOS:
		val = inet->tos;
		break;
	case IP_TTL:
	{
		struct net *net = sock_net(sk);
		val = (inet->uc_ttl == -1 ?
		       net->ipv4.sysctl_ip_default_ttl :
		       inet->uc_ttl);
		break;
	}
	case IP_HDRINCL:
		val = inet->hdrincl;
		break;
	case IP_NODEFRAG:
		val = inet->nodefrag;
		break;
	case IP_BIND_ADDRESS_NO_PORT:
		val = inet->bind_address_no_port;
		break;
	case IP_MTU_DISCOVER:
		val = inet->pmtudisc;
		break;
	case IP_MTU:
	{
		struct dst_entry *dst;
		val = 0;
		dst = sk_dst_get(sk);
		if (dst) {
			val = dst_mtu(dst);
			dst_release(dst);
		}
		if (!val) {
			release_sock(sk);
			return -ENOTCONN;
		}
		break;
	}
	case IP_RECVERR:
		val = inet->recverr;
		break;
	case IP_MULTICAST_TTL:
		val = inet->mc_ttl;
		break;
	case IP_MULTICAST_LOOP:
		val = inet->mc_loop;
		break;
	case IP_UNICAST_IF:
		val = (__force int)htonl((__u32) inet->uc_index);
		break;
	case IP_MULTICAST_IF:
	{
		struct in_addr addr;
		len = min_t(unsigned int, len, sizeof(struct in_addr));
		addr.s_addr = inet->mc_addr;
		release_sock(sk);

		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, &addr, len))
			return -EFAULT;
		return 0;
	}
	case IP_MSFILTER:
	{
		struct ip_msfilter msf;

		if (len < IP_MSFILTER_SIZE(0)) {
			err = -EINVAL;
			goto out;
		}
		if (copy_from_user(&msf, optval, IP_MSFILTER_SIZE(0))) {
			err = -EFAULT;
			goto out;
		}
		err = ip_mc_msfget(sk, &msf,
				   (struct ip_msfilter __user *)optval, optlen);
		goto out;
	}
	case MCAST_MSFILTER:
	{
		struct group_filter gsf;

		if (len < GROUP_FILTER_SIZE(0)) {
			err = -EINVAL;
			goto out;
		}
		if (copy_from_user(&gsf, optval, GROUP_FILTER_SIZE(0))) {
			err = -EFAULT;
			goto out;
		}
		err = ip_mc_gsfget(sk, &gsf,
				   (struct group_filter __user *)optval,
				   optlen);
		goto out;
	}
	case IP_MULTICAST_ALL:
		val = inet->mc_all;
		break;
	case IP_PKTOPTIONS:
	{
		struct msghdr msg;

		release_sock(sk);

		if (sk->sk_type != SOCK_STREAM)
			return -ENOPROTOOPT;

		msg.msg_control = (__force void *) optval;
		msg.msg_controllen = len;
		msg.msg_flags = flags;

		if (inet->cmsg_flags & IP_CMSG_PKTINFO) {
			struct in_pktinfo info;

			info.ipi_addr.s_addr = inet->inet_rcv_saddr;
			info.ipi_spec_dst.s_addr = inet->inet_rcv_saddr;
			info.ipi_ifindex = inet->mc_index;
			put_cmsg(&msg, SOL_IP, IP_PKTINFO, sizeof(info), &info);
		}
		if (inet->cmsg_flags & IP_CMSG_TTL) {
			int hlim = inet->mc_ttl;
			put_cmsg(&msg, SOL_IP, IP_TTL, sizeof(hlim), &hlim);
		}
		if (inet->cmsg_flags & IP_CMSG_TOS) {
			int tos = inet->rcv_tos;
			put_cmsg(&msg, SOL_IP, IP_TOS, sizeof(tos), &tos);
		}
		len -= msg.msg_controllen;
		return put_user(len, optlen);
	}
	case IP_FREEBIND:
		val = inet->freebind;
		break;
	case IP_TRANSPARENT:
		val = inet->transparent;
		break;
	case IP_MINTTL:
		val = inet->min_ttl;
		break;
	default:
		release_sock(sk);
		return -ENOPROTOOPT;
	}
	release_sock(sk);

	if (len < sizeof(int) && len > 0 && val >= 0 && val <= 255) {
		unsigned char ucval = (unsigned char)val;
		len = 1;
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, &ucval, 1))
			return -EFAULT;
	} else {
		len = min_t(unsigned int, sizeof(int), len);
		if (put_user(len, optlen))
			return -EFAULT;
		if (copy_to_user(optval, &val, len))
			return -EFAULT;
	}
	return 0;

out:
	release_sock(sk);
	if (needs_rtnl)
		rtnl_unlock();
	return err;
}
