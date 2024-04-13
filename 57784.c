static void dccp_v6_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
			u8 type, u8 code, int offset, __be32 info)
{
	const struct ipv6hdr *hdr = (const struct ipv6hdr *)skb->data;
	const struct dccp_hdr *dh;
	struct dccp_sock *dp;
	struct ipv6_pinfo *np;
	struct sock *sk;
	int err;
	__u64 seq;
	struct net *net = dev_net(skb->dev);

	/* Only need dccph_dport & dccph_sport which are the first
	 * 4 bytes in dccp header.
	 * Our caller (icmpv6_notify()) already pulled 8 bytes for us.
	 */
	BUILD_BUG_ON(offsetofend(struct dccp_hdr, dccph_sport) > 8);
	BUILD_BUG_ON(offsetofend(struct dccp_hdr, dccph_dport) > 8);
	dh = (struct dccp_hdr *)(skb->data + offset);

	sk = __inet6_lookup_established(net, &dccp_hashinfo,
					&hdr->daddr, dh->dccph_dport,
					&hdr->saddr, ntohs(dh->dccph_sport),
					inet6_iif(skb));

	if (!sk) {
		__ICMP6_INC_STATS(net, __in6_dev_get(skb->dev),
				  ICMP6_MIB_INERRORS);
		return;
	}

	if (sk->sk_state == DCCP_TIME_WAIT) {
		inet_twsk_put(inet_twsk(sk));
		return;
	}
	seq = dccp_hdr_seq(dh);
	if (sk->sk_state == DCCP_NEW_SYN_RECV)
		return dccp_req_err(sk, seq);

	bh_lock_sock(sk);
	if (sock_owned_by_user(sk))
		__NET_INC_STATS(net, LINUX_MIB_LOCKDROPPEDICMPS);

	if (sk->sk_state == DCCP_CLOSED)
		goto out;

	dp = dccp_sk(sk);
	if ((1 << sk->sk_state) & ~(DCCPF_REQUESTING | DCCPF_LISTEN) &&
	    !between48(seq, dp->dccps_awl, dp->dccps_awh)) {
		__NET_INC_STATS(net, LINUX_MIB_OUTOFWINDOWICMPS);
		goto out;
	}

	np = inet6_sk(sk);

	if (type == NDISC_REDIRECT) {
		if (!sock_owned_by_user(sk)) {
			struct dst_entry *dst = __sk_dst_check(sk, np->dst_cookie);

			if (dst)
				dst->ops->redirect(dst, sk, skb);
		}
		goto out;
	}

	if (type == ICMPV6_PKT_TOOBIG) {
		struct dst_entry *dst = NULL;

		if (!ip6_sk_accept_pmtu(sk))
			goto out;

		if (sock_owned_by_user(sk))
			goto out;
		if ((1 << sk->sk_state) & (DCCPF_LISTEN | DCCPF_CLOSED))
			goto out;

		dst = inet6_csk_update_pmtu(sk, ntohl(info));
		if (!dst)
			goto out;

		if (inet_csk(sk)->icsk_pmtu_cookie > dst_mtu(dst))
			dccp_sync_mss(sk, dst_mtu(dst));
		goto out;
	}

	icmpv6_err_convert(type, code, &err);

	/* Might be for an request_sock */
	switch (sk->sk_state) {
	case DCCP_REQUESTING:
	case DCCP_RESPOND:  /* Cannot happen.
			       It can, it SYNs are crossed. --ANK */
		if (!sock_owned_by_user(sk)) {
			__DCCP_INC_STATS(DCCP_MIB_ATTEMPTFAILS);
			sk->sk_err = err;
			/*
			 * Wake people up to see the error
			 * (see connect in sock.c)
			 */
			sk->sk_error_report(sk);
			dccp_done(sk);
		} else
			sk->sk_err_soft = err;
		goto out;
	}

	if (!sock_owned_by_user(sk) && np->recverr) {
		sk->sk_err = err;
		sk->sk_error_report(sk);
	} else
		sk->sk_err_soft = err;

out:
	bh_unlock_sock(sk);
	sock_put(sk);
}
