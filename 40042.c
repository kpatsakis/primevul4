void bt_accept_unlink(struct sock *sk)
{
	BT_DBG("sk %p state %d", sk, sk->sk_state);

	list_del_init(&bt_sk(sk)->accept_q);
	bt_sk(sk)->parent->sk_ack_backlog--;
	bt_sk(sk)->parent = NULL;
	sock_put(sk);
}
