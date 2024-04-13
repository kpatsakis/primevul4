int rfcomm_connect_ind(struct rfcomm_session *s, u8 channel, struct rfcomm_dlc **d)
{
	struct sock *sk, *parent;
	bdaddr_t src, dst;
	int result = 0;

	BT_DBG("session %p channel %d", s, channel);

	rfcomm_session_getaddr(s, &src, &dst);

	/* Check if we have socket listening on channel */
	parent = rfcomm_get_sock_by_channel(BT_LISTEN, channel, &src);
	if (!parent)
		return 0;

	bh_lock_sock(parent);

	/* Check for backlog size */
	if (sk_acceptq_is_full(parent)) {
		BT_DBG("backlog full %d", parent->sk_ack_backlog);
		goto done;
	}

	sk = rfcomm_sock_alloc(sock_net(parent), NULL, BTPROTO_RFCOMM, GFP_ATOMIC);
	if (!sk)
		goto done;

	bt_sock_reclassify_lock(sk, BTPROTO_RFCOMM);

	rfcomm_sock_init(sk, parent);
	bacpy(&bt_sk(sk)->src, &src);
	bacpy(&bt_sk(sk)->dst, &dst);
	rfcomm_pi(sk)->channel = channel;

	sk->sk_state = BT_CONFIG;
	bt_accept_enqueue(parent, sk);

	/* Accept connection and return socket DLC */
	*d = rfcomm_pi(sk)->dlc;
	result = 1;

done:
	bh_unlock_sock(parent);

	if (test_bit(BT_SK_DEFER_SETUP, &bt_sk(parent)->flags))
		parent->sk_state_change(parent);

	return result;
}
