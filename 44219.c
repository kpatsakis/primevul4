static int cm_issue_rej(struct cm_port *port,
			struct ib_mad_recv_wc *mad_recv_wc,
			enum ib_cm_rej_reason reason,
			enum cm_msg_response msg_rejected,
			void *ari, u8 ari_length)
{
	struct ib_mad_send_buf *msg = NULL;
	struct cm_rej_msg *rej_msg, *rcv_msg;
	int ret;

	ret = cm_alloc_response_msg(port, mad_recv_wc, &msg);
	if (ret)
		return ret;

	/* We just need common CM header information.  Cast to any message. */
	rcv_msg = (struct cm_rej_msg *) mad_recv_wc->recv_buf.mad;
	rej_msg = (struct cm_rej_msg *) msg->mad;

	cm_format_mad_hdr(&rej_msg->hdr, CM_REJ_ATTR_ID, rcv_msg->hdr.tid);
	rej_msg->remote_comm_id = rcv_msg->local_comm_id;
	rej_msg->local_comm_id = rcv_msg->remote_comm_id;
	cm_rej_set_msg_rejected(rej_msg, msg_rejected);
	rej_msg->reason = cpu_to_be16(reason);

	if (ari && ari_length) {
		cm_rej_set_reject_info_len(rej_msg, ari_length);
		memcpy(rej_msg->ari, ari, ari_length);
	}

	ret = ib_post_send_mad(msg, NULL);
	if (ret)
		cm_free_msg(msg);

	return ret;
}
