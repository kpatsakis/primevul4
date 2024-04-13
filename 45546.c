static void pppol2tp_session_destruct(struct sock *sk)
{
	struct l2tp_session *session = sk->sk_user_data;
	if (session) {
		sk->sk_user_data = NULL;
		BUG_ON(session->magic != L2TP_SESSION_MAGIC);
		l2tp_session_dec_refcount(session);
	}
	return;
}
