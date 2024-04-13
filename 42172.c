static void hidp_process_hid_control(struct hidp_session *session,
					unsigned char param)
{
	BT_DBG("session %p param 0x%02x", session, param);

	if (param == HIDP_CTRL_VIRTUAL_CABLE_UNPLUG) {
		/* Flush the transmit queues */
		skb_queue_purge(&session->ctrl_transmit);
		skb_queue_purge(&session->intr_transmit);

		atomic_inc(&session->terminate);
		wake_up_process(current);
	}
}
