static int sctp_send_asconf_del_ip(struct sock		*sk,
				   struct sockaddr	*addrs,
				   int			addrcnt)
{
	struct sctp_sock	*sp;
	struct sctp_endpoint	*ep;
	struct sctp_association	*asoc;
	struct sctp_transport	*transport;
	struct sctp_bind_addr	*bp;
	struct sctp_chunk	*chunk;
	union sctp_addr		*laddr;
	void			*addr_buf;
	struct sctp_af		*af;
	struct list_head	*pos, *pos1;
	struct sctp_sockaddr_entry *saddr;
	int 			i;
	int 			retval = 0;

	if (!sctp_addip_enable)
		return retval;

	sp = sctp_sk(sk);
	ep = sp->ep;

	SCTP_DEBUG_PRINTK("%s: (sk: %p, addrs: %p, addrcnt: %d)\n",
			  __FUNCTION__, sk, addrs, addrcnt);

	list_for_each(pos, &ep->asocs) {
		asoc = list_entry(pos, struct sctp_association, asocs);

		if (!asoc->peer.asconf_capable)
			continue;

		if (asoc->peer.addip_disabled_mask & SCTP_PARAM_DEL_IP)
			continue;

		if (!sctp_state(asoc, ESTABLISHED))
			continue;

		/* Check if any address in the packed array of addresses is
		 * not present in the bind address list of the association.
		 * If so, do not send the asconf chunk to its peer, but
		 * continue with other associations.
		 */
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			laddr = (union sctp_addr *)addr_buf;
			af = sctp_get_af_specific(laddr->v4.sin_family);
			if (!af) {
				retval = -EINVAL;
				goto out;
			}

			if (!sctp_assoc_lookup_laddr(asoc, laddr))
				break;

			addr_buf += af->sockaddr_len;
		}
		if (i < addrcnt)
			continue;

		/* Find one address in the association's bind address list
		 * that is not in the packed array of addresses. This is to
		 * make sure that we do not delete all the addresses in the
		 * association.
		 */
		sctp_read_lock(&asoc->base.addr_lock);
		bp = &asoc->base.bind_addr;
		laddr = sctp_find_unmatch_addr(bp, (union sctp_addr *)addrs,
					       addrcnt, sp);
		sctp_read_unlock(&asoc->base.addr_lock);
		if (!laddr)
			continue;

		chunk = sctp_make_asconf_update_ip(asoc, laddr, addrs, addrcnt,
						   SCTP_PARAM_DEL_IP);
		if (!chunk) {
			retval = -ENOMEM;
			goto out;
		}

		/* Reset use_as_src flag for the addresses in the bind address
		 * list that are to be deleted.
		 */
		sctp_local_bh_disable();
		sctp_write_lock(&asoc->base.addr_lock);
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			laddr = (union sctp_addr *)addr_buf;
			af = sctp_get_af_specific(laddr->v4.sin_family);
			list_for_each(pos1, &bp->address_list) {
				saddr = list_entry(pos1,
						   struct sctp_sockaddr_entry,
						   list);
				if (sctp_cmp_addr_exact(&saddr->a, laddr))
					saddr->use_as_src = 0;
			}
			addr_buf += af->sockaddr_len;
		}
		sctp_write_unlock(&asoc->base.addr_lock);
		sctp_local_bh_enable();

		/* Update the route and saddr entries for all the transports
		 * as some of the addresses in the bind address list are
		 * about to be deleted and cannot be used as source addresses.
		 */
		list_for_each(pos1, &asoc->peer.transport_addr_list) {
			transport = list_entry(pos1, struct sctp_transport,
					       transports);
			dst_release(transport->dst);
			sctp_transport_route(transport, NULL,
					     sctp_sk(asoc->base.sk));
		}

		retval = sctp_send_asconf(asoc, chunk);
	}
out:
	return retval;
}
