struct sk_buff *sctp_skb_recv_datagram(struct sock *sk, int flags,
				       int noblock, int *err)
{
	int error;
	struct sk_buff *skb;
	long timeo;

	timeo = sock_rcvtimeo(sk, noblock);

	pr_debug("%s: timeo:%ld, max:%ld\n", __func__, timeo,
		 MAX_SCHEDULE_TIMEOUT);

	do {
		/* Again only user level code calls this function,
		 * so nothing interrupt level
		 * will suddenly eat the receive_queue.
		 *
		 *  Look at current nfs client by the way...
		 *  However, this function was correct in any case. 8)
		 */
		if (flags & MSG_PEEK) {
			skb = skb_peek(&sk->sk_receive_queue);
			if (skb)
				refcount_inc(&skb->users);
		} else {
			skb = __skb_dequeue(&sk->sk_receive_queue);
		}

		if (skb)
			return skb;

		/* Caller is allowed not to check sk->sk_err before calling. */
		error = sock_error(sk);
		if (error)
			goto no_packet;

		if (sk->sk_shutdown & RCV_SHUTDOWN)
			break;

		if (sk_can_busy_loop(sk)) {
			sk_busy_loop(sk, noblock);

			if (!skb_queue_empty(&sk->sk_receive_queue))
				continue;
		}

		/* User doesn't want to wait.  */
		error = -EAGAIN;
		if (!timeo)
			goto no_packet;
	} while (sctp_wait_for_packet(sk, err, &timeo) == 0);

	return NULL;

no_packet:
	*err = error;
	return NULL;
}
