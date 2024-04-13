static void dccp_deliver_input_to_ccids(struct sock *sk, struct sk_buff *skb)
{
	const struct dccp_sock *dp = dccp_sk(sk);

	/* Don't deliver to RX CCID when node has shut down read end. */
	if (!(sk->sk_shutdown & RCV_SHUTDOWN))
		ccid_hc_rx_packet_recv(dp->dccps_hc_rx_ccid, sk, skb);
	/*
	 * Until the TX queue has been drained, we can not honour SHUT_WR, since
	 * we need received feedback as input to adjust congestion control.
	 */
	if (sk->sk_write_queue.qlen > 0 || !(sk->sk_shutdown & SEND_SHUTDOWN))
		ccid_hc_tx_packet_recv(dp->dccps_hc_tx_ccid, sk, skb);
}
