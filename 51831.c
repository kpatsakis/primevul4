static bool filter_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct socket *sock = sk->sk_socket;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = buf_msg(skb);
	unsigned int limit = rcvbuf_limit(sk, skb);
	int err = TIPC_OK;
	int usr = msg_user(hdr);

	if (unlikely(msg_user(hdr) == CONN_MANAGER)) {
		tipc_sk_proto_rcv(tsk, skb);
		return false;
	}

	if (unlikely(usr == SOCK_WAKEUP)) {
		kfree_skb(skb);
		tsk->link_cong = 0;
		sk->sk_write_space(sk);
		return false;
	}

	/* Drop if illegal message type */
	if (unlikely(msg_type(hdr) > TIPC_DIRECT_MSG)) {
		kfree_skb(skb);
		return false;
	}

	/* Reject if wrong message type for current socket state */
	if (unlikely(sock->state == SS_READY)) {
		if (msg_connected(hdr)) {
			err = TIPC_ERR_NO_PORT;
			goto reject;
		}
	} else if (unlikely(!filter_connect(tsk, skb))) {
		err = TIPC_ERR_NO_PORT;
		goto reject;
	}

	/* Reject message if there isn't room to queue it */
	if (unlikely(sk_rmem_alloc_get(sk) + skb->truesize >= limit)) {
		err = TIPC_ERR_OVERLOAD;
		goto reject;
	}

	/* Enqueue message */
	TIPC_SKB_CB(skb)->handle = NULL;
	__skb_queue_tail(&sk->sk_receive_queue, skb);
	skb_set_owner_r(skb, sk);

	sk->sk_data_ready(sk);
	return true;

reject:
	tipc_sk_respond(sk, skb, err);
	return false;
}
