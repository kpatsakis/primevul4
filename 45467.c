static void sco_chan_del(struct sock *sk, int err)
{
	struct sco_conn *conn;

	conn = sco_pi(sk)->conn;

	BT_DBG("sk %p, conn %p, err %d", sk, conn, err);

	if (conn) {
		sco_conn_lock(conn);
		conn->sk = NULL;
		sco_pi(sk)->conn = NULL;
		sco_conn_unlock(conn);

		if (conn->hcon)
			hci_conn_drop(conn->hcon);
	}

	sk->sk_state = BT_CLOSED;
	sk->sk_err   = err;
	sk->sk_state_change(sk);

	sock_set_flag(sk, SOCK_ZAPPED);
}
