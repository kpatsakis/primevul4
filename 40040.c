struct sock *bt_accept_dequeue(struct sock *parent, struct socket *newsock)
{
	struct list_head *p, *n;
	struct sock *sk;

	BT_DBG("parent %p", parent);

	list_for_each_safe(p, n, &bt_sk(parent)->accept_q) {
		sk = (struct sock *) list_entry(p, struct bt_sock, accept_q);

		lock_sock(sk);

		/* FIXME: Is this check still needed */
		if (sk->sk_state == BT_CLOSED) {
			release_sock(sk);
			bt_accept_unlink(sk);
			continue;
		}

		if (sk->sk_state == BT_CONNECTED || !newsock ||
		    test_bit(BT_SK_DEFER_SETUP, &bt_sk(parent)->flags)) {
			bt_accept_unlink(sk);
			if (newsock)
				sock_graft(sk, newsock);

			release_sock(sk);
			return sk;
		}

		release_sock(sk);
	}

	return NULL;
}
