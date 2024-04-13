static struct sctp_transport *sctp_addr_id2transport(struct sock *sk,
					      struct sockaddr_storage *addr,
					      sctp_assoc_t id)
{
	struct sctp_association *addr_asoc = NULL, *id_asoc = NULL;
	struct sctp_af *af = sctp_get_af_specific(addr->ss_family);
	union sctp_addr *laddr = (union sctp_addr *)addr;
	struct sctp_transport *transport;

	if (sctp_verify_addr(sk, laddr, af->sockaddr_len))
		return NULL;

	addr_asoc = sctp_endpoint_lookup_assoc(sctp_sk(sk)->ep,
					       laddr,
					       &transport);

	if (!addr_asoc)
		return NULL;

	id_asoc = sctp_id2assoc(sk, id);
	if (id_asoc && (id_asoc != addr_asoc))
		return NULL;

	sctp_get_pf_specific(sk->sk_family)->addr_to_user(sctp_sk(sk),
						(union sctp_addr *)addr);

	return transport;
}
