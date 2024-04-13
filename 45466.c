static void __sco_sock_close(struct sock *sk)
{
	BT_DBG("sk %p state %d socket %p", sk, sk->sk_state, sk->sk_socket);

	switch (sk->sk_state) {
	case BT_LISTEN:
		sco_sock_cleanup_listen(sk);
		break;

	case BT_CONNECTED:
	case BT_CONFIG:
		if (sco_pi(sk)->conn->hcon) {
			sk->sk_state = BT_DISCONN;
			sco_sock_set_timer(sk, SCO_DISCONN_TIMEOUT);
			hci_conn_drop(sco_pi(sk)->conn->hcon);
			sco_pi(sk)->conn->hcon = NULL;
		} else
			sco_chan_del(sk, ECONNRESET);
		break;

	case BT_CONNECT2:
	case BT_CONNECT:
	case BT_DISCONN:
		sco_chan_del(sk, ECONNRESET);
		break;

	default:
		sock_set_flag(sk, SOCK_ZAPPED);
		break;
	}
}
