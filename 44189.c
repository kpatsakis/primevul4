static void cm_format_dreq(struct cm_dreq_msg *dreq_msg,
			  struct cm_id_private *cm_id_priv,
			  const void *private_data,
			  u8 private_data_len)
{
	cm_format_mad_hdr(&dreq_msg->hdr, CM_DREQ_ATTR_ID,
			  cm_form_tid(cm_id_priv, CM_MSG_SEQUENCE_DREQ));
	dreq_msg->local_comm_id = cm_id_priv->id.local_id;
	dreq_msg->remote_comm_id = cm_id_priv->id.remote_id;
	cm_dreq_set_remote_qpn(dreq_msg, cm_id_priv->remote_qpn);

	if (private_data && private_data_len)
		memcpy(dreq_msg->private_data, private_data, private_data_len);
}
