static u32 filter_connect(struct tipc_sock *tsock, struct sk_buff **buf)
{
	struct socket *sock = tsock->sk.sk_socket;
	struct tipc_msg *msg = buf_msg(*buf);
	struct sock *sk = &tsock->sk;
	u32 retval = TIPC_ERR_NO_PORT;
	int res;

	if (msg_mcast(msg))
		return retval;

	switch ((int)sock->state) {
	case SS_CONNECTED:
		/* Accept only connection-based messages sent by peer */
		if (msg_connected(msg) && tipc_port_peer_msg(tsock->p, msg)) {
			if (unlikely(msg_errcode(msg))) {
				sock->state = SS_DISCONNECTING;
				__tipc_disconnect(tsock->p);
			}
			retval = TIPC_OK;
		}
		break;
	case SS_CONNECTING:
		/* Accept only ACK or NACK message */
		if (unlikely(msg_errcode(msg))) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = ECONNREFUSED;
			retval = TIPC_OK;
			break;
		}

		if (unlikely(!msg_connected(msg)))
			break;

		res = auto_connect(sock, msg);
		if (res) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = -res;
			retval = TIPC_OK;
			break;
		}

		/* If an incoming message is an 'ACK-', it should be
		 * discarded here because it doesn't contain useful
		 * data. In addition, we should try to wake up
		 * connect() routine if sleeping.
		 */
		if (msg_data_sz(msg) == 0) {
			kfree_skb(*buf);
			*buf = NULL;
			if (waitqueue_active(sk_sleep(sk)))
				wake_up_interruptible(sk_sleep(sk));
		}
		retval = TIPC_OK;
		break;
	case SS_LISTENING:
	case SS_UNCONNECTED:
		/* Accept only SYN message */
		if (!msg_connected(msg) && !(msg_errcode(msg)))
			retval = TIPC_OK;
		break;
	case SS_DISCONNECTING:
		break;
	default:
		pr_err("Unknown socket state %u\n", sock->state);
	}
	return retval;
}
