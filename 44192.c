static void cm_format_mra(struct cm_mra_msg *mra_msg,
			  struct cm_id_private *cm_id_priv,
			  enum cm_msg_response msg_mraed, u8 service_timeout,
			  const void *private_data, u8 private_data_len)
{
	cm_format_mad_hdr(&mra_msg->hdr, CM_MRA_ATTR_ID, cm_id_priv->tid);
	cm_mra_set_msg_mraed(mra_msg, msg_mraed);
	mra_msg->local_comm_id = cm_id_priv->id.local_id;
	mra_msg->remote_comm_id = cm_id_priv->id.remote_id;
	cm_mra_set_service_timeout(mra_msg, service_timeout);

	if (private_data && private_data_len)
		memcpy(mra_msg->private_data, private_data, private_data_len);
}
