static int hidp_process_data(struct hidp_session *session, struct sk_buff *skb,
				unsigned char param)
{
	int done_with_skb = 1;
	BT_DBG("session %p skb %p len %d param 0x%02x", session, skb, skb->len, param);

	switch (param) {
	case HIDP_DATA_RTYPE_INPUT:
		hidp_set_timer(session);

		if (session->input)
			hidp_input_report(session, skb);

		if (session->hid)
			hid_input_report(session->hid, HID_INPUT_REPORT, skb->data, skb->len, 0);
		break;

	case HIDP_DATA_RTYPE_OTHER:
	case HIDP_DATA_RTYPE_OUPUT:
	case HIDP_DATA_RTYPE_FEATURE:
		break;

	default:
		__hidp_send_ctrl_message(session,
			HIDP_TRANS_HANDSHAKE | HIDP_HSHK_ERR_INVALID_PARAMETER, NULL, 0);
	}

	if (test_bit(HIDP_WAITING_FOR_RETURN, &session->flags) &&
				param == session->waiting_report_type) {
		if (session->waiting_report_number < 0 ||
		    session->waiting_report_number == skb->data[0]) {
			/* hidp_get_raw_report() is waiting on this report. */
			session->report_return = skb;
			done_with_skb = 0;
			clear_bit(HIDP_WAITING_FOR_RETURN, &session->flags);
			wake_up_interruptible(&session->report_queue);
		}
	}

	return done_with_skb;
}
