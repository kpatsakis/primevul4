static int cm_apr_handler(struct cm_work *work)
{
	struct cm_id_private *cm_id_priv;
	struct cm_apr_msg *apr_msg;
	int ret;

	apr_msg = (struct cm_apr_msg *)work->mad_recv_wc->recv_buf.mad;
	cm_id_priv = cm_acquire_id(apr_msg->remote_comm_id,
				   apr_msg->local_comm_id);
	if (!cm_id_priv)
		return -EINVAL; /* Unmatched reply. */

	work->cm_event.param.apr_rcvd.ap_status = apr_msg->ap_status;
	work->cm_event.param.apr_rcvd.apr_info = &apr_msg->info;
	work->cm_event.param.apr_rcvd.info_len = apr_msg->info_length;
	work->cm_event.private_data = &apr_msg->private_data;

	spin_lock_irq(&cm_id_priv->lock);
	if (cm_id_priv->id.state != IB_CM_ESTABLISHED ||
	    (cm_id_priv->id.lap_state != IB_CM_LAP_SENT &&
	     cm_id_priv->id.lap_state != IB_CM_MRA_LAP_RCVD)) {
		spin_unlock_irq(&cm_id_priv->lock);
		goto out;
	}
	cm_id_priv->id.lap_state = IB_CM_LAP_IDLE;
	ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
	cm_id_priv->msg = NULL;

	ret = atomic_inc_and_test(&cm_id_priv->work_count);
	if (!ret)
		list_add_tail(&work->list, &cm_id_priv->work_list);
	spin_unlock_irq(&cm_id_priv->lock);

	if (ret)
		cm_process_work(cm_id_priv, work);
	else
		cm_deref_id(cm_id_priv);
	return 0;
out:
	cm_deref_id(cm_id_priv);
	return -EINVAL;
}
