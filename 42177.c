static void hidp_recv_intr_frame(struct hidp_session *session,
				struct sk_buff *skb)
{
	unsigned char hdr;

	BT_DBG("session %p skb %p len %d", session, skb, skb->len);

	hdr = skb->data[0];
	skb_pull(skb, 1);

	if (hdr == (HIDP_TRANS_DATA | HIDP_DATA_RTYPE_INPUT)) {
		hidp_set_timer(session);

		if (session->input)
			hidp_input_report(session, skb);

		if (session->hid) {
			hid_input_report(session->hid, HID_INPUT_REPORT, skb->data, skb->len, 1);
			BT_DBG("report len %d", skb->len);
		}
	} else {
		BT_DBG("Unsupported protocol header 0x%02x", hdr);
	}

	kfree_skb(skb);
}
