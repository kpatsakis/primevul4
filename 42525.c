void sctp_icmp_frag_needed(struct sock *sk, struct sctp_association *asoc,
			   struct sctp_transport *t, __u32 pmtu)
{
	if (!t || (t->pathmtu <= pmtu))
		return;

	if (sock_owned_by_user(sk)) {
		asoc->pmtu_pending = 1;
		t->pmtu_pending = 1;
		return;
	}

	if (t->param_flags & SPP_PMTUD_ENABLE) {
		/* Update transports view of the MTU */
		sctp_transport_update_pmtu(t, pmtu);

		/* Update association pmtu. */
		sctp_assoc_sync_pmtu(asoc);
	}

	/* Retransmit with the new pmtu setting.
	 * Normally, if PMTU discovery is disabled, an ICMP Fragmentation
	 * Needed will never be sent, but if a message was sent before
	 * PMTU discovery was disabled that was larger than the PMTU, it
	 * would not be fragmented, so it must be re-transmitted fragmented.
	 */
	sctp_retransmit(&asoc->outqueue, t, SCTP_RTXR_PMTUD);
}
