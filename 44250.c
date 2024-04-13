int ib_send_cm_lap(struct ib_cm_id *cm_id,
		   struct ib_sa_path_rec *alternate_path,
		   const void *private_data,
		   u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	int ret;

	if (private_data && private_data_len > IB_CM_LAP_PRIVATE_DATA_SIZE)
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_ESTABLISHED ||
	    (cm_id->lap_state != IB_CM_LAP_UNINIT &&
	     cm_id->lap_state != IB_CM_LAP_IDLE)) {
		ret = -EINVAL;
		goto out;
	}

	ret = cm_init_av_by_path(alternate_path, &cm_id_priv->alt_av);
	if (ret)
		goto out;
	cm_id_priv->alt_av.timeout =
			cm_ack_timeout(cm_id_priv->target_ack_delay,
				       cm_id_priv->alt_av.timeout - 1);

	ret = cm_alloc_msg(cm_id_priv, &msg);
	if (ret)
		goto out;

	cm_format_lap((struct cm_lap_msg *) msg->mad, cm_id_priv,
		      alternate_path, private_data, private_data_len);
	msg->timeout_ms = cm_id_priv->timeout_ms;
	msg->context[1] = (void *) (unsigned long) IB_CM_ESTABLISHED;

	ret = ib_post_send_mad(msg, NULL);
	if (ret) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		cm_free_msg(msg);
		return ret;
	}

	cm_id->lap_state = IB_CM_LAP_SENT;
	cm_id_priv->msg = msg;

out:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
