static void rfcomm_sock_kill(struct sock *sk)
{
	if (!sock_flag(sk, SOCK_ZAPPED) || sk->sk_socket)
		return;

	BT_DBG("sk %p state %d refcnt %d", sk, sk->sk_state, atomic_read(&sk->sk_refcnt));

	/* Kill poor orphan */
	bt_sock_unlink(&rfcomm_sk_list, sk);
	sock_set_flag(sk, SOCK_DEAD);
	sock_put(sk);
}
