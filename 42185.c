static void hidp_stop(struct hid_device *hid)
{
	struct hidp_session *session = hid->driver_data;

	skb_queue_purge(&session->ctrl_transmit);
	skb_queue_purge(&session->intr_transmit);

	hid->claimed = 0;
}
