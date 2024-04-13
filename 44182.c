static void cm_dup_rep_handler(struct cm_work *work)
{
	struct cm_id_private *cm_id_priv;
	struct cm_rep_msg *rep_msg;
	struct ib_mad_send_buf *msg = NULL;
	int ret;

	rep_msg = (struct cm_rep_msg *) work->mad_recv_wc->recv_buf.mad;
	cm_id_priv = cm_acquire_id(rep_msg->remote_comm_id,
				   rep_msg->local_comm_id);
	if (!cm_id_priv)
		return;

	atomic_long_inc(&work->port->counter_group[CM_RECV_DUPLICATES].
			counter[CM_REP_COUNTER]);
	ret = cm_alloc_response_msg(work->port, work->mad_recv_wc, &msg);
	if (ret)
		goto deref;

	spin_lock_irq(&cm_id_priv->lock);
	if (cm_id_priv->id.state == IB_CM_ESTABLISHED)
		cm_format_rtu((struct cm_rtu_msg *) msg->mad, cm_id_priv,
			      cm_id_priv->private_data,
			      cm_id_priv->private_data_len);
	else if (cm_id_priv->id.state == IB_CM_MRA_REP_SENT)
		cm_format_mra((struct cm_mra_msg *) msg->mad, cm_id_priv,
			      CM_MSG_RESPONSE_REP, cm_id_priv->service_timeout,
			      cm_id_priv->private_data,
			      cm_id_priv->private_data_len);
	else
		goto unlock;
	spin_unlock_irq(&cm_id_priv->lock);

	ret = ib_post_send_mad(msg, NULL);
	if (ret)
		goto free;
	goto deref;

unlock:	spin_unlock_irq(&cm_id_priv->lock);
free:	cm_free_msg(msg);
deref:	cm_deref_id(cm_id_priv);
}
