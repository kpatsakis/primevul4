static void cm_format_rtu(struct cm_rtu_msg *rtu_msg,
			  struct cm_id_private *cm_id_priv,
			  const void *private_data,
			  u8 private_data_len)
{
	cm_format_mad_hdr(&rtu_msg->hdr, CM_RTU_ATTR_ID, cm_id_priv->tid);
	rtu_msg->local_comm_id = cm_id_priv->id.local_id;
	rtu_msg->remote_comm_id = cm_id_priv->id.remote_id;

	if (private_data && private_data_len)
		memcpy(rtu_msg->private_data, private_data, private_data_len);
}
