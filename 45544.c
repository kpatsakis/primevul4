static void pppol2tp_recv(struct l2tp_session *session, struct sk_buff *skb, int data_len)
{
	struct pppol2tp_session *ps = l2tp_session_priv(session);
	struct sock *sk = NULL;

	/* If the socket is bound, send it in to PPP's input queue. Otherwise
	 * queue it on the session socket.
	 */
	sk = ps->sock;
	if (sk == NULL)
		goto no_sock;

	if (sk->sk_state & PPPOX_BOUND) {
		struct pppox_sock *po;
		l2tp_dbg(session, PPPOL2TP_MSG_DATA,
			 "%s: recv %d byte data frame, passing to ppp\n",
			 session->name, data_len);

		/* We need to forget all info related to the L2TP packet
		 * gathered in the skb as we are going to reuse the same
		 * skb for the inner packet.
		 * Namely we need to:
		 * - reset xfrm (IPSec) information as it applies to
		 *   the outer L2TP packet and not to the inner one
		 * - release the dst to force a route lookup on the inner
		 *   IP packet since skb->dst currently points to the dst
		 *   of the UDP tunnel
		 * - reset netfilter information as it doesn't apply
		 *   to the inner packet either
		 */
		secpath_reset(skb);
		skb_dst_drop(skb);
		nf_reset(skb);

		po = pppox_sk(sk);
		ppp_input(&po->chan, skb);
	} else {
		l2tp_info(session, PPPOL2TP_MSG_DATA, "%s: socket not bound\n",
			  session->name);

		/* Not bound. Nothing we can do, so discard. */
		atomic_long_inc(&session->stats.rx_errors);
		kfree_skb(skb);
	}

	return;

no_sock:
	l2tp_info(session, PPPOL2TP_MSG_DATA, "%s: no socket\n", session->name);
	kfree_skb(skb);
}
