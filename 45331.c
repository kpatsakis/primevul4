static int l2tp_ip_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len)
{
	struct sk_buff *skb;
	int rc;
	struct inet_sock *inet = inet_sk(sk);
	struct rtable *rt = NULL;
	struct flowi4 *fl4;
	int connected = 0;
	__be32 daddr;

	lock_sock(sk);

	rc = -ENOTCONN;
	if (sock_flag(sk, SOCK_DEAD))
		goto out;

	/* Get and verify the address. */
	if (msg->msg_name) {
		struct sockaddr_l2tpip *lip = (struct sockaddr_l2tpip *) msg->msg_name;
		rc = -EINVAL;
		if (msg->msg_namelen < sizeof(*lip))
			goto out;

		if (lip->l2tp_family != AF_INET) {
			rc = -EAFNOSUPPORT;
			if (lip->l2tp_family != AF_UNSPEC)
				goto out;
		}

		daddr = lip->l2tp_addr.s_addr;
	} else {
		rc = -EDESTADDRREQ;
		if (sk->sk_state != TCP_ESTABLISHED)
			goto out;

		daddr = inet->inet_daddr;
		connected = 1;
	}

	/* Allocate a socket buffer */
	rc = -ENOMEM;
	skb = sock_wmalloc(sk, 2 + NET_SKB_PAD + sizeof(struct iphdr) +
			   4 + len, 0, GFP_KERNEL);
	if (!skb)
		goto error;

	/* Reserve space for headers, putting IP header on 4-byte boundary. */
	skb_reserve(skb, 2 + NET_SKB_PAD);
	skb_reset_network_header(skb);
	skb_reserve(skb, sizeof(struct iphdr));
	skb_reset_transport_header(skb);

	/* Insert 0 session_id */
	*((__be32 *) skb_put(skb, 4)) = 0;

	/* Copy user data into skb */
	rc = memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len);
	if (rc < 0) {
		kfree_skb(skb);
		goto error;
	}

	fl4 = &inet->cork.fl.u.ip4;
	if (connected)
		rt = (struct rtable *) __sk_dst_check(sk, 0);

	rcu_read_lock();
	if (rt == NULL) {
		const struct ip_options_rcu *inet_opt;

		inet_opt = rcu_dereference(inet->inet_opt);

		/* Use correct destination address if we have options. */
		if (inet_opt && inet_opt->opt.srr)
			daddr = inet_opt->opt.faddr;

		/* If this fails, retransmit mechanism of transport layer will
		 * keep trying until route appears or the connection times
		 * itself out.
		 */
		rt = ip_route_output_ports(sock_net(sk), fl4, sk,
					   daddr, inet->inet_saddr,
					   inet->inet_dport, inet->inet_sport,
					   sk->sk_protocol, RT_CONN_FLAGS(sk),
					   sk->sk_bound_dev_if);
		if (IS_ERR(rt))
			goto no_route;
		if (connected) {
			sk_setup_caps(sk, &rt->dst);
		} else {
			skb_dst_set(skb, &rt->dst);
			goto xmit;
		}
	}

	/* We dont need to clone dst here, it is guaranteed to not disappear.
	 *  __dev_xmit_skb() might force a refcount if needed.
	 */
	skb_dst_set_noref(skb, &rt->dst);

xmit:
	/* Queue the packet to IP for output */
	rc = ip_queue_xmit(skb, &inet->cork.fl);
	rcu_read_unlock();

error:
	if (rc >= 0)
		rc = len;

out:
	release_sock(sk);
	return rc;

no_route:
	rcu_read_unlock();
	IP_INC_STATS(sock_net(sk), IPSTATS_MIB_OUTNOROUTES);
	kfree_skb(skb);
	rc = -EHOSTUNREACH;
	goto out;
}
