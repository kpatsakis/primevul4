static int sctp_send_asconf_del_ip(struct sock		*sk,
				   struct sockaddr	*addrs,
				   int			addrcnt)
{
	struct net *net = sock_net(sk);
	struct sctp_sock	*sp;
	struct sctp_endpoint	*ep;
	struct sctp_association	*asoc;
	struct sctp_transport	*transport;
	struct sctp_bind_addr	*bp;
	struct sctp_chunk	*chunk;
	union sctp_addr		*laddr;
	void			*addr_buf;
	struct sctp_af		*af;
	struct sctp_sockaddr_entry *saddr;
	int 			i;
	int 			retval = 0;
	int			stored = 0;

	chunk = NULL;
	if (!net->sctp.addip_enable)
		return retval;

	sp = sctp_sk(sk);
	ep = sp->ep;

	SCTP_DEBUG_PRINTK("%s: (sk: %p, addrs: %p, addrcnt: %d)\n",
			  __func__, sk, addrs, addrcnt);

	list_for_each_entry(asoc, &ep->asocs, asocs) {

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
			laddr = addr_buf;
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
		bp = &asoc->base.bind_addr;
		laddr = sctp_find_unmatch_addr(bp, (union sctp_addr *)addrs,
					       addrcnt, sp);
		if ((laddr == NULL) && (addrcnt == 1)) {
			if (asoc->asconf_addr_del_pending)
				continue;
			asoc->asconf_addr_del_pending =
			    kzalloc(sizeof(union sctp_addr), GFP_ATOMIC);
			if (asoc->asconf_addr_del_pending == NULL) {
				retval = -ENOMEM;
				goto out;
			}
			asoc->asconf_addr_del_pending->sa.sa_family =
				    addrs->sa_family;
			asoc->asconf_addr_del_pending->v4.sin_port =
				    htons(bp->port);
			if (addrs->sa_family == AF_INET) {
				struct sockaddr_in *sin;

				sin = (struct sockaddr_in *)addrs;
				asoc->asconf_addr_del_pending->v4.sin_addr.s_addr = sin->sin_addr.s_addr;
			} else if (addrs->sa_family == AF_INET6) {
				struct sockaddr_in6 *sin6;

				sin6 = (struct sockaddr_in6 *)addrs;
				asoc->asconf_addr_del_pending->v6.sin6_addr = sin6->sin6_addr;
			}
			SCTP_DEBUG_PRINTK_IPADDR("send_asconf_del_ip: keep the last address asoc: %p ",
			    " at %p\n", asoc, asoc->asconf_addr_del_pending,
			    asoc->asconf_addr_del_pending);
			asoc->src_out_of_asoc_ok = 1;
			stored = 1;
			goto skip_mkasconf;
		}

		/* We do not need RCU protection throughout this loop
		 * because this is done under a socket lock from the
		 * setsockopt call.
		 */
		chunk = sctp_make_asconf_update_ip(asoc, laddr, addrs, addrcnt,
						   SCTP_PARAM_DEL_IP);
		if (!chunk) {
			retval = -ENOMEM;
			goto out;
		}

skip_mkasconf:
		/* Reset use_as_src flag for the addresses in the bind address
		 * list that are to be deleted.
		 */
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			laddr = addr_buf;
			af = sctp_get_af_specific(laddr->v4.sin_family);
			list_for_each_entry(saddr, &bp->address_list, list) {
				if (sctp_cmp_addr_exact(&saddr->a, laddr))
					saddr->state = SCTP_ADDR_DEL;
			}
			addr_buf += af->sockaddr_len;
		}

		/* Update the route and saddr entries for all the transports
		 * as some of the addresses in the bind address list are
		 * about to be deleted and cannot be used as source addresses.
		 */
		list_for_each_entry(transport, &asoc->peer.transport_addr_list,
					transports) {
			dst_release(transport->dst);
			sctp_transport_route(transport, NULL,
					     sctp_sk(asoc->base.sk));
		}

		if (stored)
			/* We don't need to transmit ASCONF */
			continue;
		retval = sctp_send_asconf(asoc, chunk);
	}
out:
	return retval;
}
