static int sctp_setsockopt_peer_addr_params(struct sock *sk,
					    char __user *optval,
					    unsigned int optlen)
{
	struct sctp_paddrparams  params;
	struct sctp_transport   *trans = NULL;
	struct sctp_association *asoc = NULL;
	struct sctp_sock        *sp = sctp_sk(sk);
	int error;
	int hb_change, pmtud_change, sackdelay_change;

	if (optlen != sizeof(struct sctp_paddrparams))
		return - EINVAL;

	if (copy_from_user(&params, optval, optlen))
		return -EFAULT;

	/* Validate flags and value parameters. */
	hb_change        = params.spp_flags & SPP_HB;
	pmtud_change     = params.spp_flags & SPP_PMTUD;
	sackdelay_change = params.spp_flags & SPP_SACKDELAY;

	if (hb_change        == SPP_HB ||
	    pmtud_change     == SPP_PMTUD ||
	    sackdelay_change == SPP_SACKDELAY ||
	    params.spp_sackdelay > 500 ||
	    (params.spp_pathmtu &&
	     params.spp_pathmtu < SCTP_DEFAULT_MINSEGMENT))
		return -EINVAL;

	/* If an address other than INADDR_ANY is specified, and
	 * no transport is found, then the request is invalid.
	 */
	if (!sctp_is_any(sk, ( union sctp_addr *)&params.spp_address)) {
		trans = sctp_addr_id2transport(sk, &params.spp_address,
					       params.spp_assoc_id);
		if (!trans)
			return -EINVAL;
	}

	/* Get association, if assoc_id != 0 and the socket is a one
	 * to many style socket, and an association was not found, then
	 * the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params.spp_assoc_id);
	if (!asoc && params.spp_assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	/* Heartbeat demand can only be sent on a transport or
	 * association, but not a socket.
	 */
	if (params.spp_flags & SPP_HB_DEMAND && !trans && !asoc)
		return -EINVAL;

	/* Process parameters. */
	error = sctp_apply_peer_addr_params(&params, trans, asoc, sp,
					    hb_change, pmtud_change,
					    sackdelay_change);

	if (error)
		return error;

	/* If changes are for association, also apply parameters to each
	 * transport.
	 */
	if (!trans && asoc) {
		list_for_each_entry(trans, &asoc->peer.transport_addr_list,
				transports) {
			sctp_apply_peer_addr_params(&params, trans, asoc, sp,
						    hb_change, pmtud_change,
						    sackdelay_change);
		}
	}

	return 0;
}
