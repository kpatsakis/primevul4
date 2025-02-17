int ping_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int noblock,
		 int flags, int *addr_len)
{
	struct inet_sock *isk = inet_sk(sk);
	int family = sk->sk_family;
	struct sk_buff *skb;
	int copied, err;

	pr_debug("ping_recvmsg(sk=%p,sk->num=%u)\n", isk, isk->inet_num);

	err = -EOPNOTSUPP;
	if (flags & MSG_OOB)
		goto out;

	if (flags & MSG_ERRQUEUE)
		return inet_recv_error(sk, msg, len, addr_len);

	skb = skb_recv_datagram(sk, flags, noblock, &err);
	if (!skb)
		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	/* Don't bother checking the checksum */
	err = skb_copy_datagram_msg(skb, 0, msg, copied);
	if (err)
		goto done;

	sock_recv_timestamp(msg, sk, skb);

	/* Copy the address and add cmsg data. */
	if (family == AF_INET) {
		DECLARE_SOCKADDR(struct sockaddr_in *, sin, msg->msg_name);

		if (sin) {
			sin->sin_family = AF_INET;
			sin->sin_port = 0 /* skb->h.uh->source */;
			sin->sin_addr.s_addr = ip_hdr(skb)->saddr;
			memset(sin->sin_zero, 0, sizeof(sin->sin_zero));
			*addr_len = sizeof(*sin);
		}

		if (isk->cmsg_flags)
			ip_cmsg_recv(msg, skb);

#if IS_ENABLED(CONFIG_IPV6)
	} else if (family == AF_INET6) {
		struct ipv6_pinfo *np = inet6_sk(sk);
		struct ipv6hdr *ip6 = ipv6_hdr(skb);
		DECLARE_SOCKADDR(struct sockaddr_in6 *, sin6, msg->msg_name);

		if (sin6) {
			sin6->sin6_family = AF_INET6;
			sin6->sin6_port = 0;
			sin6->sin6_addr = ip6->saddr;
			sin6->sin6_flowinfo = 0;
			if (np->sndflow)
				sin6->sin6_flowinfo = ip6_flowinfo(ip6);
			sin6->sin6_scope_id =
				ipv6_iface_scope_id(&sin6->sin6_addr,
						    inet6_iif(skb));
			*addr_len = sizeof(*sin6);
		}

		if (inet6_sk(sk)->rxopt.all)
			pingv6_ops.ip6_datagram_recv_common_ctl(sk, msg, skb);
		if (skb->protocol == htons(ETH_P_IPV6) &&
		    inet6_sk(sk)->rxopt.all)
			pingv6_ops.ip6_datagram_recv_specific_ctl(sk, msg, skb);
		else if (skb->protocol == htons(ETH_P_IP) && isk->cmsg_flags)
			ip_cmsg_recv(msg, skb);
#endif
	} else {
		BUG();
	}

	err = copied;

done:
	skb_free_datagram(sk, skb);
out:
	pr_debug("ping_recvmsg -> %d\n", err);
	return err;
}
