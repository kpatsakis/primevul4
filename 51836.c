static int tipc_connect(struct socket *sock, struct sockaddr *dest,
			int destlen, int flags)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sockaddr_tipc *dst = (struct sockaddr_tipc *)dest;
	struct msghdr m = {NULL,};
	long timeout = (flags & O_NONBLOCK) ? 0 : tsk->conn_timeout;
	socket_state previous;
	int res = 0;

	lock_sock(sk);

	/* DGRAM/RDM connect(), just save the destaddr */
	if (sock->state == SS_READY) {
		if (dst->family == AF_UNSPEC) {
			memset(&tsk->remote, 0, sizeof(struct sockaddr_tipc));
			tsk->connected = 0;
		} else if (destlen != sizeof(struct sockaddr_tipc)) {
			res = -EINVAL;
		} else {
			memcpy(&tsk->remote, dest, destlen);
			tsk->connected = 1;
		}
		goto exit;
	}

	/*
	 * Reject connection attempt using multicast address
	 *
	 * Note: send_msg() validates the rest of the address fields,
	 *       so there's no need to do it here
	 */
	if (dst->addrtype == TIPC_ADDR_MCAST) {
		res = -EINVAL;
		goto exit;
	}

	previous = sock->state;
	switch (sock->state) {
	case SS_UNCONNECTED:
		/* Send a 'SYN-' to destination */
		m.msg_name = dest;
		m.msg_namelen = destlen;

		/* If connect is in non-blocking case, set MSG_DONTWAIT to
		 * indicate send_msg() is never blocked.
		 */
		if (!timeout)
			m.msg_flags = MSG_DONTWAIT;

		res = __tipc_sendmsg(sock, &m, 0);
		if ((res < 0) && (res != -EWOULDBLOCK))
			goto exit;

		/* Just entered SS_CONNECTING state; the only
		 * difference is that return value in non-blocking
		 * case is EINPROGRESS, rather than EALREADY.
		 */
		res = -EINPROGRESS;
	case SS_CONNECTING:
		if (previous == SS_CONNECTING)
			res = -EALREADY;
		if (!timeout)
			goto exit;
		timeout = msecs_to_jiffies(timeout);
		/* Wait until an 'ACK' or 'RST' arrives, or a timeout occurs */
		res = tipc_wait_for_connect(sock, &timeout);
		break;
	case SS_CONNECTED:
		res = -EISCONN;
		break;
	default:
		res = -EINVAL;
		break;
	}
exit:
	release_sock(sk);
	return res;
}
