static void cm_format_rej(struct cm_rej_msg *rej_msg,
			  struct cm_id_private *cm_id_priv,
			  enum ib_cm_rej_reason reason,
			  void *ari,
			  u8 ari_length,
			  const void *private_data,
			  u8 private_data_len)
{
	cm_format_mad_hdr(&rej_msg->hdr, CM_REJ_ATTR_ID, cm_id_priv->tid);
	rej_msg->remote_comm_id = cm_id_priv->id.remote_id;

	switch(cm_id_priv->id.state) {
	case IB_CM_REQ_RCVD:
		rej_msg->local_comm_id = 0;
		cm_rej_set_msg_rejected(rej_msg, CM_MSG_RESPONSE_REQ);
		break;
	case IB_CM_MRA_REQ_SENT:
		rej_msg->local_comm_id = cm_id_priv->id.local_id;
		cm_rej_set_msg_rejected(rej_msg, CM_MSG_RESPONSE_REQ);
		break;
	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
		rej_msg->local_comm_id = cm_id_priv->id.local_id;
		cm_rej_set_msg_rejected(rej_msg, CM_MSG_RESPONSE_REP);
		break;
	default:
		rej_msg->local_comm_id = cm_id_priv->id.local_id;
		cm_rej_set_msg_rejected(rej_msg, CM_MSG_RESPONSE_OTHER);
		break;
	}

	rej_msg->reason = cpu_to_be16(reason);
	if (ari && ari_length) {
		cm_rej_set_reject_info_len(rej_msg, ari_length);
		memcpy(rej_msg->ari, ari, ari_length);
	}

	if (private_data && private_data_len)
		memcpy(rej_msg->private_data, private_data, private_data_len);
}
