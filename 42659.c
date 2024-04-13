static long sctp_get_port_local(struct sock *sk, union sctp_addr *addr)
{
	struct sctp_bind_hashbucket *head; /* hash list */
	struct sctp_bind_bucket *pp; /* hash list port iterator */
	unsigned short snum;
	int ret;

	snum = ntohs(addr->v4.sin_port);

	SCTP_DEBUG_PRINTK("sctp_get_port() begins, snum=%d\n", snum);
	sctp_local_bh_disable();

	if (snum == 0) {
		/* Search for an available port.
		 *
		 * 'sctp_port_rover' was the last port assigned, so
		 * we start to search from 'sctp_port_rover +
		 * 1'. What we do is first check if port 'rover' is
		 * already in the hash table; if not, we use that; if
		 * it is, we try next.
		 */
		int low = sysctl_local_port_range[0];
		int high = sysctl_local_port_range[1];
		int remaining = (high - low) + 1;
		int rover;
		int index;

		sctp_spin_lock(&sctp_port_alloc_lock);
		rover = sctp_port_rover;
		do {
			rover++;
			if ((rover < low) || (rover > high))
				rover = low;
			index = sctp_phashfn(rover);
			head = &sctp_port_hashtable[index];
			sctp_spin_lock(&head->lock);
			for (pp = head->chain; pp; pp = pp->next)
				if (pp->port == rover)
					goto next;
			break;
		next:
			sctp_spin_unlock(&head->lock);
		} while (--remaining > 0);
		sctp_port_rover = rover;
		sctp_spin_unlock(&sctp_port_alloc_lock);

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
		head = &sctp_port_hashtable[sctp_phashfn(snum)];
		sctp_spin_lock(&head->lock);
		for (pp = head->chain; pp; pp = pp->next) {
			if (pp->port == snum)
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
		struct hlist_node *node;

		SCTP_DEBUG_PRINTK("sctp_get_port() found a possible match\n");
		if (pp->fastreuse && sk->sk_reuse)
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

			if (reuse && sk2->sk_reuse)
				continue;

			if (sctp_bind_addr_match(&ep2->base.bind_addr, addr,
						 sctp_sk(sk))) {
				ret = (long)sk2;
				goto fail_unlock;
			}
		}
		SCTP_DEBUG_PRINTK("sctp_get_port(): Found a match\n");
	}
pp_not_found:
	/* If there was a hash table miss, create a new port.  */
	ret = 1;
	if (!pp && !(pp = sctp_bucket_create(head, snum)))
		goto fail_unlock;

	/* In either case (hit or miss), make sure fastreuse is 1 only
	 * if sk->sk_reuse is too (that is, if the caller requested
	 * SO_REUSEADDR on this socket -sk-).
	 */
	if (hlist_empty(&pp->owner))
		pp->fastreuse = sk->sk_reuse ? 1 : 0;
	else if (pp->fastreuse && !sk->sk_reuse)
		pp->fastreuse = 0;

	/* We are set, so fill up all the data in the hash table
	 * entry, tie the socket list information with the rest of the
	 * sockets FIXME: Blurry, NPI (ipg).
	 */
success:
	inet_sk(sk)->num = snum;
	if (!sctp_sk(sk)->bind_hash) {
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
