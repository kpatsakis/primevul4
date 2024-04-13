struct sctp_association *sctp_id2assoc(struct sock *sk, sctp_assoc_t id)
{
	struct sctp_association *asoc = NULL;

	/* If this is not a UDP-style socket, assoc id should be ignored. */
	if (!sctp_style(sk, UDP)) {
		/* Return NULL if the socket state is not ESTABLISHED. It
		 * could be a TCP-style listening socket or a socket which
		 * hasn't yet called connect() to establish an association.
		 */
		if (!sctp_sstate(sk, ESTABLISHED) && !sctp_sstate(sk, CLOSING))
			return NULL;

		/* Get the first and the only association from the list. */
		if (!list_empty(&sctp_sk(sk)->ep->asocs))
			asoc = list_entry(sctp_sk(sk)->ep->asocs.next,
					  struct sctp_association, asocs);
		return asoc;
	}

	/* Otherwise this is a UDP-style socket. */
	if (!id || (id == (sctp_assoc_t)-1))
		return NULL;

	spin_lock_bh(&sctp_assocs_id_lock);
	asoc = (struct sctp_association *)idr_find(&sctp_assocs_id, (int)id);
	spin_unlock_bh(&sctp_assocs_id_lock);

	if (!asoc || (asoc->base.sk != sk) || asoc->base.dead)
		return NULL;

	return asoc;
}
