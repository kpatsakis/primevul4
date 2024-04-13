int ip_cmsg_send(struct sock *sk, struct msghdr *msg, struct ipcm_cookie *ipc,
		 bool allow_ipv6)
{
	int err, val;
	struct cmsghdr *cmsg;
	struct net *net = sock_net(sk);

	for_each_cmsghdr(cmsg, msg) {
		if (!CMSG_OK(msg, cmsg))
			return -EINVAL;
#if IS_ENABLED(CONFIG_IPV6)
		if (allow_ipv6 &&
		    cmsg->cmsg_level == SOL_IPV6 &&
		    cmsg->cmsg_type == IPV6_PKTINFO) {
			struct in6_pktinfo *src_info;

			if (cmsg->cmsg_len < CMSG_LEN(sizeof(*src_info)))
				return -EINVAL;
			src_info = (struct in6_pktinfo *)CMSG_DATA(cmsg);
			if (!ipv6_addr_v4mapped(&src_info->ipi6_addr))
				return -EINVAL;
			ipc->oif = src_info->ipi6_ifindex;
			ipc->addr = src_info->ipi6_addr.s6_addr32[3];
			continue;
		}
#endif
		if (cmsg->cmsg_level == SOL_SOCKET) {
			err = __sock_cmsg_send(sk, msg, cmsg, &ipc->sockc);
			if (err)
				return err;
			continue;
		}

		if (cmsg->cmsg_level != SOL_IP)
			continue;
		switch (cmsg->cmsg_type) {
		case IP_RETOPTS:
			err = cmsg->cmsg_len - CMSG_ALIGN(sizeof(struct cmsghdr));

			/* Our caller is responsible for freeing ipc->opt */
			err = ip_options_get(net, &ipc->opt, CMSG_DATA(cmsg),
					     err < 40 ? err : 40);
			if (err)
				return err;
			break;
		case IP_PKTINFO:
		{
			struct in_pktinfo *info;
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct in_pktinfo)))
				return -EINVAL;
			info = (struct in_pktinfo *)CMSG_DATA(cmsg);
			ipc->oif = info->ipi_ifindex;
			ipc->addr = info->ipi_spec_dst.s_addr;
			break;
		}
		case IP_TTL:
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
				return -EINVAL;
			val = *(int *)CMSG_DATA(cmsg);
			if (val < 1 || val > 255)
				return -EINVAL;
			ipc->ttl = val;
			break;
		case IP_TOS:
			if (cmsg->cmsg_len == CMSG_LEN(sizeof(int)))
				val = *(int *)CMSG_DATA(cmsg);
			else if (cmsg->cmsg_len == CMSG_LEN(sizeof(u8)))
				val = *(u8 *)CMSG_DATA(cmsg);
			else
				return -EINVAL;
			if (val < 0 || val > 255)
				return -EINVAL;
			ipc->tos = val;
			ipc->priority = rt_tos2priority(ipc->tos);
			break;

		default:
			return -EINVAL;
		}
	}
	return 0;
}
