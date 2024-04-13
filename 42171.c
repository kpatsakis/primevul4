static void hidp_process_handshake(struct hidp_session *session,
					unsigned char param)
{
	BT_DBG("session %p param 0x%02x", session, param);
	session->output_report_success = 0; /* default condition */

	switch (param) {
	case HIDP_HSHK_SUCCESSFUL:
		/* FIXME: Call into SET_ GET_ handlers here */
		session->output_report_success = 1;
		break;

	case HIDP_HSHK_NOT_READY:
	case HIDP_HSHK_ERR_INVALID_REPORT_ID:
	case HIDP_HSHK_ERR_UNSUPPORTED_REQUEST:
	case HIDP_HSHK_ERR_INVALID_PARAMETER:
		if (test_and_clear_bit(HIDP_WAITING_FOR_RETURN, &session->flags))
			wake_up_interruptible(&session->report_queue);

		/* FIXME: Call into SET_ GET_ handlers here */
		break;

	case HIDP_HSHK_ERR_UNKNOWN:
		break;

	case HIDP_HSHK_ERR_FATAL:
		/* Device requests a reboot, as this is the only way this error
		 * can be recovered. */
		__hidp_send_ctrl_message(session,
			HIDP_TRANS_HID_CONTROL | HIDP_CTRL_SOFT_RESET, NULL, 0);
		break;

	default:
		__hidp_send_ctrl_message(session,
			HIDP_TRANS_HANDSHAKE | HIDP_HSHK_ERR_INVALID_PARAMETER, NULL, 0);
		break;
	}

	/* Wake up the waiting thread. */
	if (test_and_clear_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags))
		wake_up_interruptible(&session->report_queue);
}
