SCTP_STATIC struct sock *sctp_accept(struct sock *sk, int flags, int *err)
{
	struct sctp_sock *sp;
	struct sctp_endpoint *ep;
	struct sock *newsk = NULL;
	struct sctp_association *asoc;
	long timeo;
	int error = 0;

	sctp_lock_sock(sk);

	sp = sctp_sk(sk);
	ep = sp->ep;

	if (!sctp_style(sk, TCP)) {
		error = -EOPNOTSUPP;
		goto out;
	}

	if (!sctp_sstate(sk, LISTENING)) {
		error = -EINVAL;
		goto out;
	}

	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);

	error = sctp_wait_for_accept(sk, timeo);
	if (error)
		goto out;

	/* We treat the list of associations on the endpoint as the accept
	 * queue and pick the first association on the list.
	 */
	asoc = list_entry(ep->asocs.next, struct sctp_association, asocs);

	newsk = sp->pf->create_accept_sk(sk, asoc);
	if (!newsk) {
		error = -ENOMEM;
		goto out;
	}

	/* Populate the fields of the newsk from the oldsk and migrate the
	 * asoc to the newsk.
	 */
	sctp_sock_migrate(sk, newsk, asoc, SCTP_SOCKET_TCP);

out:
	sctp_release_sock(sk);
	*err = error;
	return newsk;
}
