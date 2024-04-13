static void pppol2tp_session_close(struct l2tp_session *session)
{
	struct pppol2tp_session *ps = l2tp_session_priv(session);
	struct sock *sk = ps->sock;
	struct socket *sock = sk->sk_socket;

	BUG_ON(session->magic != L2TP_SESSION_MAGIC);


	if (sock) {
		inet_shutdown(sock, 2);
		/* Don't let the session go away before our socket does */
		l2tp_session_inc_refcount(session);
	}
	return;
}
