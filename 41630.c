static long sctp_get_port_local(struct sock *sk, union sctp_addr *addr)
{
	struct sctp_bind_hashbucket *head; /* hash list */
	struct sctp_bind_bucket *pp; /* hash list port iterator */
	struct hlist_node *node;
	unsigned short snum;
	int ret;

	snum = ntohs(addr->v4.sin_port);

	SCTP_DEBUG_PRINTK("sctp_get_port() begins, snum=%d\n", snum);
	sctp_local_bh_disable();

	if (snum == 0) {
		/* Search for an available port. */
		int low, high, remaining, index;
		unsigned int rover;

		inet_get_local_port_range(&low, &high);
		remaining = (high - low) + 1;
		rover = net_random() % remaining + low;

		do {
			rover++;
			if ((rover < low) || (rover > high))
				rover = low;
			if (inet_is_reserved_local_port(rover))
				continue;
			index = sctp_phashfn(sock_net(sk), rover);
			head = &sctp_port_hashtable[index];
			sctp_spin_lock(&head->lock);
			sctp_for_each_hentry(pp, node, &head->chain)
				if ((pp->port == rover) &&
				    net_eq(sock_net(sk), pp->net))
					goto next;
			break;
		next:
			sctp_spin_unlock(&head->lock);
		} while (--remaining > 0);

		/* Exhausted local port range during search? */
		ret = 1;
		if (remaining <= 0)
			goto fail;

		/* OK, here is the one we will use.  HEAD (the port
		 * hash table list entry) is non-NULL and we hold it's
		 * mutex.
		 */
		snum = rover;
	} else {
		/* We are given an specific port number; we verify
		 * that it is not being used. If it is used, we will
		 * exahust the search in the hash list corresponding
		 * to the port number (snum) - we detect that with the
		 * port iterator, pp being NULL.
		 */
		head = &sctp_port_hashtable[sctp_phashfn(sock_net(sk), snum)];
		sctp_spin_lock(&head->lock);
		sctp_for_each_hentry(pp, node, &head->chain) {
			if ((pp->port == snum) && net_eq(pp->net, sock_net(sk)))
				goto pp_found;
		}
	}
	pp = NULL;
	goto pp_not_found;
pp_found:
	if (!hlist_empty(&pp->owner)) {
		/* We had a port hash table hit - there is an
		 * available port (pp != NULL) and it is being
		 * used by other socket (pp->owner not empty); that other
		 * socket is going to be sk2.
		 */
		int reuse = sk->sk_reuse;
		struct sock *sk2;

		SCTP_DEBUG_PRINTK("sctp_get_port() found a possible match\n");
		if (pp->fastreuse && sk->sk_reuse &&
			sk->sk_state != SCTP_SS_LISTENING)
			goto success;

		/* Run through the list of sockets bound to the port
		 * (pp->port) [via the pointers bind_next and
		 * bind_pprev in the struct sock *sk2 (pp->sk)]. On each one,
		 * we get the endpoint they describe and run through
		 * the endpoint's list of IP (v4 or v6) addresses,
		 * comparing each of the addresses with the address of
		 * the socket sk. If we find a match, then that means
		 * that this port/socket (sk) combination are already
		 * in an endpoint.
		 */
		sk_for_each_bound(sk2, node, &pp->owner) {
			struct sctp_endpoint *ep2;
			ep2 = sctp_sk(sk2)->ep;

			if (sk == sk2 ||
			    (reuse && sk2->sk_reuse &&
			     sk2->sk_state != SCTP_SS_LISTENING))
				continue;

			if (sctp_bind_addr_conflict(&ep2->base.bind_addr, addr,
						 sctp_sk(sk2), sctp_sk(sk))) {
				ret = (long)sk2;
				goto fail_unlock;
			}
		}
		SCTP_DEBUG_PRINTK("sctp_get_port(): Found a match\n");
	}
pp_not_found:
	/* If there was a hash table miss, create a new port.  */
	ret = 1;
	if (!pp && !(pp = sctp_bucket_create(head, sock_net(sk), snum)))
		goto fail_unlock;

	/* In either case (hit or miss), make sure fastreuse is 1 only
	 * if sk->sk_reuse is too (that is, if the caller requested
	 * SO_REUSEADDR on this socket -sk-).
	 */
	if (hlist_empty(&pp->owner)) {
		if (sk->sk_reuse && sk->sk_state != SCTP_SS_LISTENING)
			pp->fastreuse = 1;
		else
			pp->fastreuse = 0;
	} else if (pp->fastreuse &&
		(!sk->sk_reuse || sk->sk_state == SCTP_SS_LISTENING))
		pp->fastreuse = 0;

	/* We are set, so fill up all the data in the hash table
	 * entry, tie the socket list information with the rest of the
	 * sockets FIXME: Blurry, NPI (ipg).
	 */
success:
	if (!sctp_sk(sk)->bind_hash) {
		inet_sk(sk)->inet_num = snum;
		sk_add_bind_node(sk, &pp->owner);
		sctp_sk(sk)->bind_hash = pp;
	}
	ret = 0;

fail_unlock:
	sctp_spin_unlock(&head->lock);

fail:
	sctp_local_bh_enable();
	return ret;
}
