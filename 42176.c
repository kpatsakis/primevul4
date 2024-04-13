static void hidp_recv_ctrl_frame(struct hidp_session *session,
					struct sk_buff *skb)
{
	unsigned char hdr, type, param;
	int free_skb = 1;

	BT_DBG("session %p skb %p len %d", session, skb, skb->len);

	hdr = skb->data[0];
	skb_pull(skb, 1);

	type = hdr & HIDP_HEADER_TRANS_MASK;
	param = hdr & HIDP_HEADER_PARAM_MASK;

	switch (type) {
	case HIDP_TRANS_HANDSHAKE:
		hidp_process_handshake(session, param);
		break;

	case HIDP_TRANS_HID_CONTROL:
		hidp_process_hid_control(session, param);
		break;

	case HIDP_TRANS_DATA:
		free_skb = hidp_process_data(session, skb, param);
		break;

	default:
		__hidp_send_ctrl_message(session,
			HIDP_TRANS_HANDSHAKE | HIDP_HSHK_ERR_UNSUPPORTED_REQUEST, NULL, 0);
		break;
	}

	if (free_skb)
		kfree_skb(skb);
}
