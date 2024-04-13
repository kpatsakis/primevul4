static void rfcomm_sk_state_change(struct rfcomm_dlc *d, int err)
{
	struct sock *sk = d->owner, *parent;
	unsigned long flags;

	if (!sk)
		return;

	BT_DBG("dlc %p state %ld err %d", d, d->state, err);

	local_irq_save(flags);
	bh_lock_sock(sk);

	if (err)
		sk->sk_err = err;

	sk->sk_state = d->state;

	parent = bt_sk(sk)->parent;
	if (parent) {
		if (d->state == BT_CLOSED) {
			sock_set_flag(sk, SOCK_ZAPPED);
			bt_accept_unlink(sk);
		}
		parent->sk_data_ready(parent, 0);
	} else {
		if (d->state == BT_CONNECTED)
			rfcomm_session_getaddr(d->session, &bt_sk(sk)->src, NULL);
		sk->sk_state_change(sk);
	}

	bh_unlock_sock(sk);
	local_irq_restore(flags);

	if (parent && sock_flag(sk, SOCK_ZAPPED)) {
		/* We have to drop DLC lock here, otherwise
		 * rfcomm_sock_destruct() will dead lock. */
		rfcomm_dlc_unlock(d);
		rfcomm_sock_kill(sk);
		rfcomm_dlc_lock(d);
	}
}
