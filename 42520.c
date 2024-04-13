int sctp_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_chunk *chunk = SCTP_INPUT_CB(skb)->chunk;
	struct sctp_inq *inqueue = &chunk->rcvr->inqueue;
	struct sctp_ep_common *rcvr = NULL;
	int backloged = 0;

	rcvr = chunk->rcvr;

	/* If the rcvr is dead then the association or endpoint
	 * has been deleted and we can safely drop the chunk
	 * and refs that we are holding.
	 */
	if (rcvr->dead) {
		sctp_chunk_free(chunk);
		goto done;
	}

	if (unlikely(rcvr->sk != sk)) {
		/* In this case, the association moved from one socket to
		 * another.  We are currently sitting on the backlog of the
		 * old socket, so we need to move.
		 * However, since we are here in the process context we
		 * need to take make sure that the user doesn't own
		 * the new socket when we process the packet.
		 * If the new socket is user-owned, queue the chunk to the
		 * backlog of the new socket without dropping any refs.
		 * Otherwise, we can safely push the chunk on the inqueue.
		 */

		sk = rcvr->sk;
		sctp_bh_lock_sock(sk);

		if (sock_owned_by_user(sk)) {
			sk_add_backlog(sk, skb);
			backloged = 1;
		} else
			sctp_inq_push(inqueue, chunk);

		sctp_bh_unlock_sock(sk);

		/* If the chunk was backloged again, don't drop refs */
		if (backloged)
			return 0;
	} else {
		sctp_inq_push(inqueue, chunk);
	}

done:
	/* Release the refs we took in sctp_add_backlog */
	if (SCTP_EP_TYPE_ASSOCIATION == rcvr->type)
		sctp_association_put(sctp_assoc(rcvr));
	else if (SCTP_EP_TYPE_SOCKET == rcvr->type)
		sctp_endpoint_put(sctp_ep(rcvr));
	else
		BUG();

	return 0;
}
