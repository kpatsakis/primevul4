int dccp_rcv_established(struct sock *sk, struct sk_buff *skb,
			 const struct dccp_hdr *dh, const unsigned int len)
{
	if (dccp_check_seqno(sk, skb))
		goto discard;

	if (dccp_parse_options(sk, NULL, skb))
		return 1;

	dccp_handle_ackvec_processing(sk, skb);
	dccp_deliver_input_to_ccids(sk, skb);

	return __dccp_rcv_established(sk, skb, dh, len);
discard:
	__kfree_skb(skb);
	return 0;
}
