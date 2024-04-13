int ib_send_cm_mra(struct ib_cm_id *cm_id,
		   u8 service_timeout,
		   const void *private_data,
		   u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	enum ib_cm_state cm_state;
	enum ib_cm_lap_state lap_state;
	enum cm_msg_response msg_response;
	void *data;
	unsigned long flags;
	int ret;

	if (private_data && private_data_len > IB_CM_MRA_PRIVATE_DATA_SIZE)
		return -EINVAL;

	data = cm_copy_private_data(private_data, private_data_len);
	if (IS_ERR(data))
		return PTR_ERR(data);

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);

	spin_lock_irqsave(&cm_id_priv->lock, flags);
	switch(cm_id_priv->id.state) {
	case IB_CM_REQ_RCVD:
		cm_state = IB_CM_MRA_REQ_SENT;
		lap_state = cm_id->lap_state;
		msg_response = CM_MSG_RESPONSE_REQ;
		break;
	case IB_CM_REP_RCVD:
		cm_state = IB_CM_MRA_REP_SENT;
		lap_state = cm_id->lap_state;
		msg_response = CM_MSG_RESPONSE_REP;
		break;
	case IB_CM_ESTABLISHED:
		if (cm_id->lap_state == IB_CM_LAP_RCVD) {
			cm_state = cm_id->state;
			lap_state = IB_CM_MRA_LAP_SENT;
			msg_response = CM_MSG_RESPONSE_OTHER;
			break;
		}
	default:
		ret = -EINVAL;
		goto error1;
	}

	if (!(service_timeout & IB_CM_MRA_FLAG_DELAY)) {
		ret = cm_alloc_msg(cm_id_priv, &msg);
		if (ret)
			goto error1;

		cm_format_mra((struct cm_mra_msg *) msg->mad, cm_id_priv,
			      msg_response, service_timeout,
			      private_data, private_data_len);
		ret = ib_post_send_mad(msg, NULL);
		if (ret)
			goto error2;
	}

	cm_id->state = cm_state;
	cm_id->lap_state = lap_state;
	cm_id_priv->service_timeout = service_timeout;
	cm_set_private_data(cm_id_priv, data, private_data_len);
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return 0;

error1:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	kfree(data);
	return ret;

error2:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	kfree(data);
	cm_free_msg(msg);
	return ret;
}
