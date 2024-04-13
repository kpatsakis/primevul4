int ib_send_cm_dreq(struct ib_cm_id *cm_id,
		    const void *private_data,
		    u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	int ret;

	if (private_data && private_data_len > IB_CM_DREQ_PRIVATE_DATA_SIZE)
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_ESTABLISHED) {
		ret = -EINVAL;
		goto out;
	}

	if (cm_id->lap_state == IB_CM_LAP_SENT ||
	    cm_id->lap_state == IB_CM_MRA_LAP_RCVD)
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);

	ret = cm_alloc_msg(cm_id_priv, &msg);
	if (ret) {
		cm_enter_timewait(cm_id_priv);
		goto out;
	}

	cm_format_dreq((struct cm_dreq_msg *) msg->mad, cm_id_priv,
		       private_data, private_data_len);
	msg->timeout_ms = cm_id_priv->timeout_ms;
	msg->context[1] = (void *) (unsigned long) IB_CM_DREQ_SENT;

	ret = ib_post_send_mad(msg, NULL);
	if (ret) {
		cm_enter_timewait(cm_id_priv);
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		cm_free_msg(msg);
		return ret;
	}

	cm_id->state = IB_CM_DREQ_SENT;
	cm_id_priv->msg = msg;
out:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
