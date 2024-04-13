int ib_send_cm_sidr_rep(struct ib_cm_id *cm_id,
			struct ib_cm_sidr_rep_param *param)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	int ret;

	if ((param->info && param->info_length > IB_CM_SIDR_REP_INFO_LENGTH) ||
	    (param->private_data &&
	     param->private_data_len > IB_CM_SIDR_REP_PRIVATE_DATA_SIZE))
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_SIDR_REQ_RCVD) {
		ret = -EINVAL;
		goto error;
	}

	ret = cm_alloc_msg(cm_id_priv, &msg);
	if (ret)
		goto error;

	cm_format_sidr_rep((struct cm_sidr_rep_msg *) msg->mad, cm_id_priv,
			   param);
	ret = ib_post_send_mad(msg, NULL);
	if (ret) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		cm_free_msg(msg);
		return ret;
	}
	cm_id->state = IB_CM_IDLE;
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);

	spin_lock_irqsave(&cm.lock, flags);
	rb_erase(&cm_id_priv->sidr_id_node, &cm.remote_sidr_table);
	spin_unlock_irqrestore(&cm.lock, flags);
	return 0;

error:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
