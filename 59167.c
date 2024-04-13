static void dccp_handle_ackvec_processing(struct sock *sk, struct sk_buff *skb)
{
	struct dccp_ackvec *av = dccp_sk(sk)->dccps_hc_rx_ackvec;

	if (av == NULL)
		return;
	if (DCCP_SKB_CB(skb)->dccpd_ack_seq != DCCP_PKT_WITHOUT_ACK_SEQ)
		dccp_ackvec_clear_state(av, DCCP_SKB_CB(skb)->dccpd_ack_seq);
	dccp_ackvec_input(av, skb);
}
