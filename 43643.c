static void sctp_select_active_and_retran_path(struct sctp_association *asoc)
{
	struct sctp_transport *trans, *trans_pri = NULL, *trans_sec = NULL;
	struct sctp_transport *trans_pf = NULL;

	/* Look for the two most recently used active transports. */
	list_for_each_entry(trans, &asoc->peer.transport_addr_list,
			    transports) {
		/* Skip uninteresting transports. */
		if (trans->state == SCTP_INACTIVE ||
		    trans->state == SCTP_UNCONFIRMED)
			continue;
		/* Keep track of the best PF transport from our
		 * list in case we don't find an active one.
		 */
		if (trans->state == SCTP_PF) {
			trans_pf = sctp_trans_elect_best(trans, trans_pf);
			continue;
		}
		/* For active transports, pick the most recent ones. */
		if (trans_pri == NULL ||
		    ktime_after(trans->last_time_heard,
				trans_pri->last_time_heard)) {
			trans_sec = trans_pri;
			trans_pri = trans;
		} else if (trans_sec == NULL ||
			   ktime_after(trans->last_time_heard,
				       trans_sec->last_time_heard)) {
			trans_sec = trans;
		}
	}

	/* RFC 2960 6.4 Multi-Homed SCTP Endpoints
	 *
	 * By default, an endpoint should always transmit to the primary
	 * path, unless the SCTP user explicitly specifies the
	 * destination transport address (and possibly source transport
	 * address) to use. [If the primary is active but not most recent,
	 * bump the most recently used transport.]
	 */
	if ((asoc->peer.primary_path->state == SCTP_ACTIVE ||
	     asoc->peer.primary_path->state == SCTP_UNKNOWN) &&
	     asoc->peer.primary_path != trans_pri) {
		trans_sec = trans_pri;
		trans_pri = asoc->peer.primary_path;
	}

	/* We did not find anything useful for a possible retransmission
	 * path; either primary path that we found is the the same as
	 * the current one, or we didn't generally find an active one.
	 */
	if (trans_sec == NULL)
		trans_sec = trans_pri;

	/* If we failed to find a usable transport, just camp on the
	 * active or pick a PF iff it's the better choice.
	 */
	if (trans_pri == NULL) {
		trans_pri = sctp_trans_elect_best(asoc->peer.active_path, trans_pf);
		trans_sec = trans_pri;
	}

	/* Set the active and retran transports. */
	asoc->peer.active_path = trans_pri;
	asoc->peer.retran_path = trans_sec;
}
