static void cm_format_drep(struct cm_drep_msg *drep_msg,
			  struct cm_id_private *cm_id_priv,
			  const void *private_data,
			  u8 private_data_len)
{
	cm_format_mad_hdr(&drep_msg->hdr, CM_DREP_ATTR_ID, cm_id_priv->tid);
	drep_msg->local_comm_id = cm_id_priv->id.local_id;
	drep_msg->remote_comm_id = cm_id_priv->id.remote_id;

	if (private_data && private_data_len)
		memcpy(drep_msg->private_data, private_data, private_data_len);
}
