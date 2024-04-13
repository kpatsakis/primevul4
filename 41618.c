SCTP_STATIC void sctp_close(struct sock *sk, long timeout)
{
	struct net *net = sock_net(sk);
	struct sctp_endpoint *ep;
	struct sctp_association *asoc;
	struct list_head *pos, *temp;
	unsigned int data_was_unread;

	SCTP_DEBUG_PRINTK("sctp_close(sk: 0x%p, timeout:%ld)\n", sk, timeout);

	sctp_lock_sock(sk);
	sk->sk_shutdown = SHUTDOWN_MASK;
	sk->sk_state = SCTP_SS_CLOSING;

	ep = sctp_sk(sk)->ep;

	/* Clean up any skbs sitting on the receive queue.  */
	data_was_unread = sctp_queue_purge_ulpevents(&sk->sk_receive_queue);
	data_was_unread += sctp_queue_purge_ulpevents(&sctp_sk(sk)->pd_lobby);

	/* Walk all associations on an endpoint.  */
	list_for_each_safe(pos, temp, &ep->asocs) {
		asoc = list_entry(pos, struct sctp_association, asocs);

		if (sctp_style(sk, TCP)) {
			/* A closed association can still be in the list if
			 * it belongs to a TCP-style listening socket that is
			 * not yet accepted. If so, free it. If not, send an
			 * ABORT or SHUTDOWN based on the linger options.
			 */
			if (sctp_state(asoc, CLOSED)) {
				sctp_unhash_established(asoc);
				sctp_association_free(asoc);
				continue;
			}
		}

		if (data_was_unread || !skb_queue_empty(&asoc->ulpq.lobby) ||
		    !skb_queue_empty(&asoc->ulpq.reasm) ||
		    (sock_flag(sk, SOCK_LINGER) && !sk->sk_lingertime)) {
			struct sctp_chunk *chunk;

			chunk = sctp_make_abort_user(asoc, NULL, 0);
			if (chunk)
				sctp_primitive_ABORT(net, asoc, chunk);
		} else
			sctp_primitive_SHUTDOWN(net, asoc, NULL);
	}

	/* On a TCP-style socket, block for at most linger_time if set. */
	if (sctp_style(sk, TCP) && timeout)
		sctp_wait_for_close(sk, timeout);

	/* This will run the backlog queue.  */
	sctp_release_sock(sk);

	/* Supposedly, no process has access to the socket, but
	 * the net layers still may.
	 */
	sctp_local_bh_disable();
	sctp_bh_lock_sock(sk);

	/* Hold the sock, since sk_common_release() will put sock_put()
	 * and we have just a little more cleanup.
	 */
	sock_hold(sk);
	sk_common_release(sk);

	sctp_bh_unlock_sock(sk);
	sctp_local_bh_enable();

	sock_put(sk);

	SCTP_DBG_OBJCNT_DEC(sock);
}
