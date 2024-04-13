static bool is_filtered_packet(struct sock *sk, struct sk_buff *skb)
{
	struct hci_filter *flt;
	int flt_type, flt_event;

	/* Apply filter */
	flt = &hci_pi(sk)->filter;

	if (bt_cb(skb)->pkt_type == HCI_VENDOR_PKT)
		flt_type = 0;
	else
		flt_type = bt_cb(skb)->pkt_type & HCI_FLT_TYPE_BITS;

	if (!test_bit(flt_type, &flt->type_mask))
		return true;

	/* Extra filter for event packets only */
	if (bt_cb(skb)->pkt_type != HCI_EVENT_PKT)
		return false;

	flt_event = (*(__u8 *)skb->data & HCI_FLT_EVENT_BITS);

	if (!hci_test_bit(flt_event, &flt->event_mask))
		return true;

	/* Check filter only when opcode is set */
	if (!flt->opcode)
		return false;

	if (flt_event == HCI_EV_CMD_COMPLETE &&
	    flt->opcode != get_unaligned((__le16 *)(skb->data + 3)))
		return true;

	if (flt_event == HCI_EV_CMD_STATUS &&
	    flt->opcode != get_unaligned((__le16 *)(skb->data + 4)))
		return true;

	return false;
}
