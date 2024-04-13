int ib_send_cm_drep(struct ib_cm_id *cm_id,
		    const void *private_data,
		    u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	void *data;
	int ret;

	if (private_data && private_data_len > IB_CM_DREP_PRIVATE_DATA_SIZE)
		return -EINVAL;

	data = cm_copy_private_data(private_data, private_data_len);
	if (IS_ERR(data))
		return PTR_ERR(data);

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_DREQ_RCVD) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		kfree(data);
		return -EINVAL;
	}

	cm_set_private_data(cm_id_priv, data, private_data_len);
	cm_enter_timewait(cm_id_priv);

	ret = cm_alloc_msg(cm_id_priv, &msg);
	if (ret)
		goto out;

	cm_format_drep((struct cm_drep_msg *) msg->mad, cm_id_priv,
		       private_data, private_data_len);

	ret = ib_post_send_mad(msg, NULL);
	if (ret) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		cm_free_msg(msg);
		return ret;
	}

out:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
