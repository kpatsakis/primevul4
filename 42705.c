SCTP_STATIC void sctp_shutdown(struct sock *sk, int how)
{
	struct sctp_endpoint *ep;
	struct sctp_association *asoc;

	if (!sctp_style(sk, TCP))
		return;

	if (how & SEND_SHUTDOWN) {
		ep = sctp_sk(sk)->ep;
		if (!list_empty(&ep->asocs)) {
			asoc = list_entry(ep->asocs.next,
					  struct sctp_association, asocs);
			sctp_primitive_SHUTDOWN(asoc, NULL);
		}
	}
}
