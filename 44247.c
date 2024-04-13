int ib_send_cm_apr(struct ib_cm_id *cm_id,
		   enum ib_cm_apr_status status,
		   void *info,
		   u8 info_length,
		   const void *private_data,
		   u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	int ret;

	if ((private_data && private_data_len > IB_CM_APR_PRIVATE_DATA_SIZE) ||
	    (info && info_length > IB_CM_APR_INFO_LENGTH))
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_ESTABLISHED ||
	    (cm_id->lap_state != IB_CM_LAP_RCVD &&
	     cm_id->lap_state != IB_CM_MRA_LAP_SENT)) {
		ret = -EINVAL;
		goto out;
	}

	ret = cm_alloc_msg(cm_id_priv, &msg);
	if (ret)
		goto out;

	cm_format_apr((struct cm_apr_msg *) msg->mad, cm_id_priv, status,
		      info, info_length, private_data, private_data_len);
	ret = ib_post_send_mad(msg, NULL);
	if (ret) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		cm_free_msg(msg);
		return ret;
	}

	cm_id->lap_state = IB_CM_LAP_IDLE;
out:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
