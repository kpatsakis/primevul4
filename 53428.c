static int rfcomm_sock_sendmsg(struct socket *sock, struct msghdr *msg,
			       size_t len)
{
	struct sock *sk = sock->sk;
	struct rfcomm_dlc *d = rfcomm_pi(sk)->dlc;
	struct sk_buff *skb;
	int sent;

	if (test_bit(RFCOMM_DEFER_SETUP, &d->flags))
		return -ENOTCONN;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	if (sk->sk_shutdown & SEND_SHUTDOWN)
		return -EPIPE;

	BT_DBG("sock %p, sk %p", sock, sk);

	lock_sock(sk);

	sent = bt_sock_wait_ready(sk, msg->msg_flags);
	if (sent)
		goto done;

	while (len) {
		size_t size = min_t(size_t, len, d->mtu);
		int err;

		skb = sock_alloc_send_skb(sk, size + RFCOMM_SKB_RESERVE,
				msg->msg_flags & MSG_DONTWAIT, &err);
		if (!skb) {
			if (sent == 0)
				sent = err;
			break;
		}
		skb_reserve(skb, RFCOMM_SKB_HEAD_RESERVE);

		err = memcpy_from_msg(skb_put(skb, size), msg, size);
		if (err) {
			kfree_skb(skb);
			if (sent == 0)
				sent = err;
			break;
		}

		skb->priority = sk->sk_priority;

		err = rfcomm_dlc_send(d, skb);
		if (err < 0) {
			kfree_skb(skb);
			if (sent == 0)
				sent = err;
			break;
		}

		sent += size;
		len  -= size;
	}

done:
	release_sock(sk);

	return sent;
}
