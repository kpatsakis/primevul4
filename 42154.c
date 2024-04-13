int hidp_del_connection(struct hidp_conndel_req *req)
{
	struct hidp_session *session;
	int err = 0;

	BT_DBG("");

	down_read(&hidp_session_sem);

	session = __hidp_get_session(&req->bdaddr);
	if (session) {
		if (req->flags & (1 << HIDP_VIRTUAL_CABLE_UNPLUG)) {
			hidp_send_ctrl_message(session,
				HIDP_TRANS_HID_CONTROL | HIDP_CTRL_VIRTUAL_CABLE_UNPLUG, NULL, 0);
		} else {
			/* Flush the transmit queues */
			skb_queue_purge(&session->ctrl_transmit);
			skb_queue_purge(&session->intr_transmit);

			atomic_inc(&session->terminate);
			wake_up_process(session->task);
		}
	} else
		err = -ENOENT;

	up_read(&hidp_session_sem);
	return err;
}
