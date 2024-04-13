static void sctp_shutdown(struct sock *sk, int how)
{
	struct net *net = sock_net(sk);
	struct sctp_endpoint *ep;

	if (!sctp_style(sk, TCP))
		return;

	ep = sctp_sk(sk)->ep;
	if (how & SEND_SHUTDOWN && !list_empty(&ep->asocs)) {
		struct sctp_association *asoc;

		sk->sk_state = SCTP_SS_CLOSING;
		asoc = list_entry(ep->asocs.next,
				  struct sctp_association, asocs);
		sctp_primitive_SHUTDOWN(net, asoc, NULL);
	}
}
