static int pppol2tp_connect(struct socket *sock, struct sockaddr *uservaddr,
			    int sockaddr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct sockaddr_pppol2tp *sp = (struct sockaddr_pppol2tp *) uservaddr;
	struct pppox_sock *po = pppox_sk(sk);
	struct l2tp_session *session = NULL;
	struct l2tp_tunnel *tunnel;
	struct pppol2tp_session *ps;
	struct dst_entry *dst;
	struct l2tp_session_cfg cfg = { 0, };
	int error = 0;
	u32 tunnel_id, peer_tunnel_id;
	u32 session_id, peer_session_id;
	int ver = 2;
	int fd;

	lock_sock(sk);

	error = -EINVAL;
	if (sp->sa_protocol != PX_PROTO_OL2TP)
		goto end;

	/* Check for already bound sockets */
	error = -EBUSY;
	if (sk->sk_state & PPPOX_CONNECTED)
		goto end;

	/* We don't supporting rebinding anyway */
	error = -EALREADY;
	if (sk->sk_user_data)
		goto end; /* socket is already attached */

	/* Get params from socket address. Handle L2TPv2 and L2TPv3.
	 * This is nasty because there are different sockaddr_pppol2tp
	 * structs for L2TPv2, L2TPv3, over IPv4 and IPv6. We use
	 * the sockaddr size to determine which structure the caller
	 * is using.
	 */
	peer_tunnel_id = 0;
	if (sockaddr_len == sizeof(struct sockaddr_pppol2tp)) {
		fd = sp->pppol2tp.fd;
		tunnel_id = sp->pppol2tp.s_tunnel;
		peer_tunnel_id = sp->pppol2tp.d_tunnel;
		session_id = sp->pppol2tp.s_session;
		peer_session_id = sp->pppol2tp.d_session;
	} else if (sockaddr_len == sizeof(struct sockaddr_pppol2tpv3)) {
		struct sockaddr_pppol2tpv3 *sp3 =
			(struct sockaddr_pppol2tpv3 *) sp;
		ver = 3;
		fd = sp3->pppol2tp.fd;
		tunnel_id = sp3->pppol2tp.s_tunnel;
		peer_tunnel_id = sp3->pppol2tp.d_tunnel;
		session_id = sp3->pppol2tp.s_session;
		peer_session_id = sp3->pppol2tp.d_session;
	} else if (sockaddr_len == sizeof(struct sockaddr_pppol2tpin6)) {
		struct sockaddr_pppol2tpin6 *sp6 =
			(struct sockaddr_pppol2tpin6 *) sp;
		fd = sp6->pppol2tp.fd;
		tunnel_id = sp6->pppol2tp.s_tunnel;
		peer_tunnel_id = sp6->pppol2tp.d_tunnel;
		session_id = sp6->pppol2tp.s_session;
		peer_session_id = sp6->pppol2tp.d_session;
	} else if (sockaddr_len == sizeof(struct sockaddr_pppol2tpv3in6)) {
		struct sockaddr_pppol2tpv3in6 *sp6 =
			(struct sockaddr_pppol2tpv3in6 *) sp;
		ver = 3;
		fd = sp6->pppol2tp.fd;
		tunnel_id = sp6->pppol2tp.s_tunnel;
		peer_tunnel_id = sp6->pppol2tp.d_tunnel;
		session_id = sp6->pppol2tp.s_session;
		peer_session_id = sp6->pppol2tp.d_session;
	} else {
		error = -EINVAL;
		goto end; /* bad socket address */
	}

	/* Don't bind if tunnel_id is 0 */
	error = -EINVAL;
	if (tunnel_id == 0)
		goto end;

	tunnel = l2tp_tunnel_find(sock_net(sk), tunnel_id);

	/* Special case: create tunnel context if session_id and
	 * peer_session_id is 0. Otherwise look up tunnel using supplied
	 * tunnel id.
	 */
	if ((session_id == 0) && (peer_session_id == 0)) {
		if (tunnel == NULL) {
			struct l2tp_tunnel_cfg tcfg = {
				.encap = L2TP_ENCAPTYPE_UDP,
				.debug = 0,
			};
			error = l2tp_tunnel_create(sock_net(sk), fd, ver, tunnel_id, peer_tunnel_id, &tcfg, &tunnel);
			if (error < 0)
				goto end;
		}
	} else {
		/* Error if we can't find the tunnel */
		error = -ENOENT;
		if (tunnel == NULL)
			goto end;

		/* Error if socket is not prepped */
		if (tunnel->sock == NULL)
			goto end;
	}

	if (tunnel->recv_payload_hook == NULL)
		tunnel->recv_payload_hook = pppol2tp_recv_payload_hook;

	if (tunnel->peer_tunnel_id == 0)
		tunnel->peer_tunnel_id = peer_tunnel_id;

	/* Create session if it doesn't already exist. We handle the
	 * case where a session was previously created by the netlink
	 * interface by checking that the session doesn't already have
	 * a socket and its tunnel socket are what we expect. If any
	 * of those checks fail, return EEXIST to the caller.
	 */
	session = l2tp_session_find(sock_net(sk), tunnel, session_id);
	if (session == NULL) {
		/* Default MTU must allow space for UDP/L2TP/PPP
		 * headers.
		 */
		cfg.mtu = cfg.mru = 1500 - PPPOL2TP_HEADER_OVERHEAD;

		/* Allocate and initialize a new session context. */
		session = l2tp_session_create(sizeof(struct pppol2tp_session),
					      tunnel, session_id,
					      peer_session_id, &cfg);
		if (session == NULL) {
			error = -ENOMEM;
			goto end;
		}
	} else {
		ps = l2tp_session_priv(session);
		error = -EEXIST;
		if (ps->sock != NULL)
			goto end;

		/* consistency checks */
		if (ps->tunnel_sock != tunnel->sock)
			goto end;
	}

	/* Associate session with its PPPoL2TP socket */
	ps = l2tp_session_priv(session);
	ps->owner	     = current->pid;
	ps->sock	     = sk;
	ps->tunnel_sock = tunnel->sock;

	session->recv_skb	= pppol2tp_recv;
	session->session_close	= pppol2tp_session_close;
#if defined(CONFIG_L2TP_DEBUGFS) || defined(CONFIG_L2TP_DEBUGFS_MODULE)
	session->show		= pppol2tp_show;
#endif

	/* We need to know each time a skb is dropped from the reorder
	 * queue.
	 */
	session->ref = pppol2tp_session_sock_hold;
	session->deref = pppol2tp_session_sock_put;

	/* If PMTU discovery was enabled, use the MTU that was discovered */
	dst = sk_dst_get(sk);
	if (dst != NULL) {
		u32 pmtu = dst_mtu(__sk_dst_get(sk));
		if (pmtu != 0)
			session->mtu = session->mru = pmtu -
				PPPOL2TP_HEADER_OVERHEAD;
		dst_release(dst);
	}

	/* Special case: if source & dest session_id == 0x0000, this
	 * socket is being created to manage the tunnel. Just set up
	 * the internal context for use by ioctl() and sockopt()
	 * handlers.
	 */
	if ((session->session_id == 0) &&
	    (session->peer_session_id == 0)) {
		error = 0;
		goto out_no_ppp;
	}

	/* The only header we need to worry about is the L2TP
	 * header. This size is different depending on whether
	 * sequence numbers are enabled for the data channel.
	 */
	po->chan.hdrlen = PPPOL2TP_L2TP_HDR_SIZE_NOSEQ;

	po->chan.private = sk;
	po->chan.ops	 = &pppol2tp_chan_ops;
	po->chan.mtu	 = session->mtu;

	error = ppp_register_net_channel(sock_net(sk), &po->chan);
	if (error)
		goto end;

out_no_ppp:
	/* This is how we get the session context from the socket. */
	sk->sk_user_data = session;
	sk->sk_state = PPPOX_CONNECTED;
	l2tp_info(session, PPPOL2TP_MSG_CONTROL, "%s: created\n",
		  session->name);

end:
	release_sock(sk);

	return error;
}
