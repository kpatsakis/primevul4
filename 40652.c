sctp_disposition_t sctp_sf_backbeat_8_3(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	union sctp_addr from_addr;
	struct sctp_transport *link;
	sctp_sender_hb_info_t *hbinfo;
	unsigned long max_interval;

	if (!sctp_vtag_verify(chunk, asoc))
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

	/* Make sure that the HEARTBEAT-ACK chunk has a valid length.  */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_chunkhdr_t) +
					    sizeof(sctp_sender_hb_info_t)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);

	hbinfo = (sctp_sender_hb_info_t *) chunk->skb->data;
	/* Make sure that the length of the parameter is what we expect */
	if (ntohs(hbinfo->param_hdr.length) !=
				    sizeof(sctp_sender_hb_info_t)) {
		return SCTP_DISPOSITION_DISCARD;
	}

	from_addr = hbinfo->daddr;
	link = sctp_assoc_lookup_paddr(asoc, &from_addr);

	/* This should never happen, but lets log it if so.  */
	if (unlikely(!link)) {
		if (from_addr.sa.sa_family == AF_INET6) {
			net_warn_ratelimited("%s association %p could not find address %pI6\n",
					     __func__,
					     asoc,
					     &from_addr.v6.sin6_addr);
		} else {
			net_warn_ratelimited("%s association %p could not find address %pI4\n",
					     __func__,
					     asoc,
					     &from_addr.v4.sin_addr.s_addr);
		}
		return SCTP_DISPOSITION_DISCARD;
	}

	/* Validate the 64-bit random nonce. */
	if (hbinfo->hb_nonce != link->hb_nonce)
		return SCTP_DISPOSITION_DISCARD;

	max_interval = link->hbinterval + link->rto;

	/* Check if the timestamp looks valid.  */
	if (time_after(hbinfo->sent_at, jiffies) ||
	    time_after(jiffies, hbinfo->sent_at + max_interval)) {
		SCTP_DEBUG_PRINTK("%s: HEARTBEAT ACK with invalid timestamp "
				  "received for transport: %p\n",
				   __func__, link);
		return SCTP_DISPOSITION_DISCARD;
	}

	/* 8.3 Upon the receipt of the HEARTBEAT ACK, the sender of
	 * the HEARTBEAT should clear the error counter of the
	 * destination transport address to which the HEARTBEAT was
	 * sent and mark the destination transport address as active if
	 * it is not so marked.
	 */
	sctp_add_cmd_sf(commands, SCTP_CMD_TRANSPORT_ON, SCTP_TRANSPORT(link));

	return SCTP_DISPOSITION_CONSUME;
}
