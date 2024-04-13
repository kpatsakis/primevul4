static void hidp_process_ctrl_transmit(struct hidp_session *session)
{
	struct sk_buff *skb;

	BT_DBG("session %p", session);

	while ((skb = skb_dequeue(&session->ctrl_transmit))) {
		if (hidp_send_frame(session->ctrl_sock, skb->data, skb->len) < 0) {
			skb_queue_head(&session->ctrl_transmit, skb);
			break;
		}

		hidp_set_timer(session);
		kfree_skb(skb);
	}
}
