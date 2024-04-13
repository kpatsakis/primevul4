static int sctp_send_asconf_add_ip(struct sock		*sk,
				   struct sockaddr	*addrs,
				   int 			addrcnt)
{
	struct sctp_sock		*sp;
	struct sctp_endpoint		*ep;
	struct sctp_association		*asoc;
	struct sctp_bind_addr		*bp;
	struct sctp_chunk		*chunk;
	struct sctp_sockaddr_entry	*laddr;
	union sctp_addr			*addr;
	union sctp_addr			saveaddr;
	void				*addr_buf;
	struct sctp_af			*af;
	struct list_head		*pos;
	struct list_head		*p;
	int 				i;
	int 				retval = 0;

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

		if (asoc->peer.addip_disabled_mask & SCTP_PARAM_ADD_IP)
			continue;

		if (!sctp_state(asoc, ESTABLISHED))
			continue;

		/* Check if any address in the packed array of addresses is
		 * in the bind address list of the association. If so,
		 * do not send the asconf chunk to its peer, but continue with
		 * other associations.
		 */
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			addr = (union sctp_addr *)addr_buf;
			af = sctp_get_af_specific(addr->v4.sin_family);
			if (!af) {
				retval = -EINVAL;
				goto out;
			}

			if (sctp_assoc_lookup_laddr(asoc, addr))
				break;

			addr_buf += af->sockaddr_len;
		}
		if (i < addrcnt)
			continue;

		/* Use the first address in bind addr list of association as
		 * Address Parameter of ASCONF CHUNK.
		 */
		sctp_read_lock(&asoc->base.addr_lock);
		bp = &asoc->base.bind_addr;
		p = bp->address_list.next;
		laddr = list_entry(p, struct sctp_sockaddr_entry, list);
		sctp_read_unlock(&asoc->base.addr_lock);

		chunk = sctp_make_asconf_update_ip(asoc, &laddr->a, addrs,
						   addrcnt, SCTP_PARAM_ADD_IP);
		if (!chunk) {
			retval = -ENOMEM;
			goto out;
		}

		retval = sctp_send_asconf(asoc, chunk);
		if (retval)
			goto out;

		/* Add the new addresses to the bind address list with
		 * use_as_src set to 0.
		 */
		sctp_local_bh_disable();
		sctp_write_lock(&asoc->base.addr_lock);
		addr_buf = addrs;
		for (i = 0; i < addrcnt; i++) {
			addr = (union sctp_addr *)addr_buf;
			af = sctp_get_af_specific(addr->v4.sin_family);
			memcpy(&saveaddr, addr, af->sockaddr_len);
			retval = sctp_add_bind_addr(bp, &saveaddr, 0,
						    GFP_ATOMIC);
			addr_buf += af->sockaddr_len;
		}
		sctp_write_unlock(&asoc->base.addr_lock);
		sctp_local_bh_enable();
	}

out:
	return retval;
}
