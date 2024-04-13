int ib_send_cm_rej(struct ib_cm_id *cm_id,
		   enum ib_cm_rej_reason reason,
		   void *ari,
		   u8 ari_length,
		   const void *private_data,
		   u8 private_data_len)
{
	struct cm_id_private *cm_id_priv;
	struct ib_mad_send_buf *msg;
	unsigned long flags;
	int ret;

	if ((private_data && private_data_len > IB_CM_REJ_PRIVATE_DATA_SIZE) ||
	    (ari && ari_length > IB_CM_REJ_ARI_LENGTH))
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);

	spin_lock_irqsave(&cm_id_priv->lock, flags);
	switch (cm_id->state) {
	case IB_CM_REQ_SENT:
	case IB_CM_MRA_REQ_RCVD:
	case IB_CM_REQ_RCVD:
	case IB_CM_MRA_REQ_SENT:
	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
		ret = cm_alloc_msg(cm_id_priv, &msg);
		if (!ret)
			cm_format_rej((struct cm_rej_msg *) msg->mad,
				      cm_id_priv, reason, ari, ari_length,
				      private_data, private_data_len);

		cm_reset_to_idle(cm_id_priv);
		break;
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
		ret = cm_alloc_msg(cm_id_priv, &msg);
		if (!ret)
			cm_format_rej((struct cm_rej_msg *) msg->mad,
				      cm_id_priv, reason, ari, ari_length,
				      private_data, private_data_len);

		cm_enter_timewait(cm_id_priv);
		break;
	default:
		ret = -EINVAL;
		goto out;
	}

	if (ret)
		goto out;

	ret = ib_post_send_mad(msg, NULL);
	if (ret)
		cm_free_msg(msg);

out:	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
