int sctp_copy_local_addr_list(struct net *net, struct sctp_bind_addr *bp,
			      sctp_scope_t scope, gfp_t gfp, int copy_flags)
{
	struct sctp_sockaddr_entry *addr;
	int error = 0;

	rcu_read_lock();
	list_for_each_entry_rcu(addr, &net->sctp.local_addr_list, list) {
		if (!addr->valid)
			continue;
		if (sctp_in_scope(net, &addr->a, scope)) {
			/* Now that the address is in scope, check to see if
			 * the address type is really supported by the local
			 * sock as well as the remote peer.
			 */
			if ((((AF_INET == addr->a.sa.sa_family) &&
			      (copy_flags & SCTP_ADDR4_PEERSUPP))) ||
			    (((AF_INET6 == addr->a.sa.sa_family) &&
			      (copy_flags & SCTP_ADDR6_ALLOWED) &&
			      (copy_flags & SCTP_ADDR6_PEERSUPP)))) {
				error = sctp_add_bind_addr(bp, &addr->a,
						    SCTP_ADDR_SRC, GFP_ATOMIC);
				if (error)
					goto end_copy;
			}
		}
	}

end_copy:
	rcu_read_unlock();
	return error;
}
