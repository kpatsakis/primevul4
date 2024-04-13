void ip_cmsg_recv_offset(struct msghdr *msg, struct sock *sk,
			 struct sk_buff *skb, int tlen, int offset)
{
	struct inet_sock *inet = inet_sk(sk);
	unsigned int flags = inet->cmsg_flags;

	/* Ordered by supposed usage frequency */
	if (flags & IP_CMSG_PKTINFO) {
		ip_cmsg_recv_pktinfo(msg, skb);

		flags &= ~IP_CMSG_PKTINFO;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_TTL) {
		ip_cmsg_recv_ttl(msg, skb);

		flags &= ~IP_CMSG_TTL;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_TOS) {
		ip_cmsg_recv_tos(msg, skb);

		flags &= ~IP_CMSG_TOS;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_RECVOPTS) {
		ip_cmsg_recv_opts(msg, skb);

		flags &= ~IP_CMSG_RECVOPTS;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_RETOPTS) {
		ip_cmsg_recv_retopts(msg, skb);

		flags &= ~IP_CMSG_RETOPTS;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_PASSSEC) {
		ip_cmsg_recv_security(msg, skb);

		flags &= ~IP_CMSG_PASSSEC;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_ORIGDSTADDR) {
		ip_cmsg_recv_dstaddr(msg, skb);

		flags &= ~IP_CMSG_ORIGDSTADDR;
		if (!flags)
			return;
	}

	if (flags & IP_CMSG_CHECKSUM)
		ip_cmsg_recv_checksum(msg, skb, tlen, offset);

	if (flags & IP_CMSG_RECVFRAGSIZE)
		ip_cmsg_recv_fragsize(msg, skb);
}
