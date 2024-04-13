static int tipc_recvmsg(struct socket *sock, struct msghdr *m, size_t buf_len,
			int flags)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *buf;
	struct tipc_msg *msg;
	long timeo;
	unsigned int sz;
	u32 err;
	int res, hlen;

	/* Catch invalid receive requests */
	if (unlikely(!buf_len))
		return -EINVAL;

	lock_sock(sk);

	if (unlikely(sock->state == SS_UNCONNECTED)) {
		res = -ENOTCONN;
		goto exit;
	}

	timeo = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);
restart:

	/* Look for a message in receive queue; wait if necessary */
	res = tipc_wait_for_rcvmsg(sock, &timeo);
	if (res)
		goto exit;

	/* Look at first message in receive queue */
	buf = skb_peek(&sk->sk_receive_queue);
	msg = buf_msg(buf);
	sz = msg_data_sz(msg);
	hlen = msg_hdr_sz(msg);
	err = msg_errcode(msg);

	/* Discard an empty non-errored message & try again */
	if ((!sz) && (!err)) {
		tsk_advance_rx_queue(sk);
		goto restart;
	}

	/* Capture sender's address (optional) */
	set_orig_addr(m, msg);

	/* Capture ancillary data (optional) */
	res = tipc_sk_anc_data_recv(m, msg, tsk);
	if (res)
		goto exit;

	/* Capture message data (if valid) & compute return value (always) */
	if (!err) {
		if (unlikely(buf_len < sz)) {
			sz = buf_len;
			m->msg_flags |= MSG_TRUNC;
		}
		res = skb_copy_datagram_msg(buf, hlen, m, sz);
		if (res)
			goto exit;
		res = sz;
	} else {
		if ((sock->state == SS_READY) ||
		    ((err == TIPC_CONN_SHUTDOWN) || m->msg_control))
			res = 0;
		else
			res = -ECONNRESET;
	}

	if (unlikely(flags & MSG_PEEK))
		goto exit;

	if (likely(sock->state != SS_READY)) {
		tsk->rcv_unacked += tsk_inc(tsk, hlen + sz);
		if (unlikely(tsk->rcv_unacked >= (tsk->rcv_win / 4)))
			tipc_sk_send_ack(tsk);
	}
	tsk_advance_rx_queue(sk);
exit:
	release_sock(sk);
	return res;
}
