static void tipc_sk_proto_rcv(struct tipc_sock *tsk, struct sk_buff *skb)
{
	struct sock *sk = &tsk->sk;
	struct tipc_msg *hdr = buf_msg(skb);
	int mtyp = msg_type(hdr);
	bool conn_cong;

	/* Ignore if connection cannot be validated: */
	if (!tsk_peer_msg(tsk, hdr))
		goto exit;

	tsk->probing_state = TIPC_CONN_OK;

	if (mtyp == CONN_PROBE) {
		msg_set_type(hdr, CONN_PROBE_REPLY);
		tipc_sk_respond(sk, skb, TIPC_OK);
		return;
	} else if (mtyp == CONN_ACK) {
		conn_cong = tsk_conn_cong(tsk);
		tsk->snt_unacked -= msg_conn_ack(hdr);
		if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL)
			tsk->snd_win = msg_adv_win(hdr);
		if (conn_cong)
			sk->sk_write_space(sk);
	} else if (mtyp != CONN_PROBE_REPLY) {
		pr_warn("Received unknown CONN_PROTO msg\n");
	}
exit:
	kfree_skb(skb);
}
