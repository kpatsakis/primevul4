static int __sctp_connect(struct sock* sk,
			  struct sockaddr *kaddrs,
			  int addrs_size,
			  sctp_assoc_t *assoc_id)
{
	struct net *net = sock_net(sk);
	struct sctp_sock *sp;
	struct sctp_endpoint *ep;
	struct sctp_association *asoc = NULL;
	struct sctp_association *asoc2;
	struct sctp_transport *transport;
	union sctp_addr to;
	struct sctp_af *af;
	sctp_scope_t scope;
	long timeo;
	int err = 0;
	int addrcnt = 0;
	int walk_size = 0;
	union sctp_addr *sa_addr = NULL;
	void *addr_buf;
	unsigned short port;
	unsigned int f_flags = 0;

	sp = sctp_sk(sk);
	ep = sp->ep;

	/* connect() cannot be done on a socket that is already in ESTABLISHED
	 * state - UDP-style peeled off socket or a TCP-style socket that
	 * is already connected.
	 * It cannot be done even on a TCP-style listening socket.
	 */
	if (sctp_sstate(sk, ESTABLISHED) ||
	    (sctp_style(sk, TCP) && sctp_sstate(sk, LISTENING))) {
		err = -EISCONN;
		goto out_free;
	}

	/* Walk through the addrs buffer and count the number of addresses. */
	addr_buf = kaddrs;
	while (walk_size < addrs_size) {
		if (walk_size + sizeof(sa_family_t) > addrs_size) {
			err = -EINVAL;
			goto out_free;
		}

		sa_addr = addr_buf;
		af = sctp_get_af_specific(sa_addr->sa.sa_family);

		/* If the address family is not supported or if this address
		 * causes the address buffer to overflow return EINVAL.
		 */
		if (!af || (walk_size + af->sockaddr_len) > addrs_size) {
			err = -EINVAL;
			goto out_free;
		}

		port = ntohs(sa_addr->v4.sin_port);

		/* Save current address so we can work with it */
		memcpy(&to, sa_addr, af->sockaddr_len);

		err = sctp_verify_addr(sk, &to, af->sockaddr_len);
		if (err)
			goto out_free;

		/* Make sure the destination port is correctly set
		 * in all addresses.
		 */
		if (asoc && asoc->peer.port && asoc->peer.port != port)
			goto out_free;


		/* Check if there already is a matching association on the
		 * endpoint (other than the one created here).
		 */
		asoc2 = sctp_endpoint_lookup_assoc(ep, &to, &transport);
		if (asoc2 && asoc2 != asoc) {
			if (asoc2->state >= SCTP_STATE_ESTABLISHED)
				err = -EISCONN;
			else
				err = -EALREADY;
			goto out_free;
		}

		/* If we could not find a matching association on the endpoint,
		 * make sure that there is no peeled-off association matching
		 * the peer address even on another socket.
		 */
		if (sctp_endpoint_is_peeled_off(ep, &to)) {
			err = -EADDRNOTAVAIL;
			goto out_free;
		}

		if (!asoc) {
			/* If a bind() or sctp_bindx() is not called prior to
			 * an sctp_connectx() call, the system picks an
			 * ephemeral port and will choose an address set
			 * equivalent to binding with a wildcard address.
			 */
			if (!ep->base.bind_addr.port) {
				if (sctp_autobind(sk)) {
					err = -EAGAIN;
					goto out_free;
				}
			} else {
				/*
				 * If an unprivileged user inherits a 1-many
				 * style socket with open associations on a
				 * privileged port, it MAY be permitted to
				 * accept new associations, but it SHOULD NOT
				 * be permitted to open new associations.
				 */
				if (ep->base.bind_addr.port < PROT_SOCK &&
				    !ns_capable(net->user_ns, CAP_NET_BIND_SERVICE)) {
					err = -EACCES;
					goto out_free;
				}
			}

			scope = sctp_scope(&to);
			asoc = sctp_association_new(ep, sk, scope, GFP_KERNEL);
			if (!asoc) {
				err = -ENOMEM;
				goto out_free;
			}

			err = sctp_assoc_set_bind_addr_from_ep(asoc, scope,
							      GFP_KERNEL);
			if (err < 0) {
				goto out_free;
			}

		}

		/* Prime the peer's transport structures.  */
		transport = sctp_assoc_add_peer(asoc, &to, GFP_KERNEL,
						SCTP_UNKNOWN);
		if (!transport) {
			err = -ENOMEM;
			goto out_free;
		}

		addrcnt++;
		addr_buf += af->sockaddr_len;
		walk_size += af->sockaddr_len;
	}

	/* In case the user of sctp_connectx() wants an association
	 * id back, assign one now.
	 */
	if (assoc_id) {
		err = sctp_assoc_set_id(asoc, GFP_KERNEL);
		if (err < 0)
			goto out_free;
	}

	err = sctp_primitive_ASSOCIATE(net, asoc, NULL);
	if (err < 0) {
		goto out_free;
	}

	/* Initialize sk's dport and daddr for getpeername() */
	inet_sk(sk)->inet_dport = htons(asoc->peer.port);
	af = sctp_get_af_specific(sa_addr->sa.sa_family);
	af->to_sk_daddr(sa_addr, sk);
	sk->sk_err = 0;

	/* in-kernel sockets don't generally have a file allocated to them
	 * if all they do is call sock_create_kern().
	 */
	if (sk->sk_socket->file)
		f_flags = sk->sk_socket->file->f_flags;

	timeo = sock_sndtimeo(sk, f_flags & O_NONBLOCK);

	err = sctp_wait_for_connect(asoc, &timeo);
	if ((err == 0 || err == -EINPROGRESS) && assoc_id)
		*assoc_id = asoc->assoc_id;

	/* Don't free association on exit. */
	asoc = NULL;

out_free:

	SCTP_DEBUG_PRINTK("About to exit __sctp_connect() free asoc: %p"
			  " kaddrs: %p err: %d\n",
			  asoc, kaddrs, err);
	if (asoc) {
		/* sctp_primitive_ASSOCIATE may have added this association
		 * To the hash table, try to unhash it, just in case, its a noop
		 * if it wasn't hashed so we're safe
		 */
		sctp_unhash_established(asoc);
		sctp_association_free(asoc);
	}
	return err;
}
