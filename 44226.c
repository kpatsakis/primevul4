static int cm_rej_handler(struct cm_work *work)
{
	struct cm_id_private *cm_id_priv;
	struct cm_rej_msg *rej_msg;
	int ret;

	rej_msg = (struct cm_rej_msg *)work->mad_recv_wc->recv_buf.mad;
	cm_id_priv = cm_acquire_rejected_id(rej_msg);
	if (!cm_id_priv)
		return -EINVAL;

	cm_format_rej_event(work);

	spin_lock_irq(&cm_id_priv->lock);
	switch (cm_id_priv->id.state) {
	case IB_CM_REQ_SENT:
	case IB_CM_MRA_REQ_RCVD:
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		/* fall through */
	case IB_CM_REQ_RCVD:
	case IB_CM_MRA_REQ_SENT:
		if (__be16_to_cpu(rej_msg->reason) == IB_CM_REJ_STALE_CONN)
			cm_enter_timewait(cm_id_priv);
		else
			cm_reset_to_idle(cm_id_priv);
		break;
	case IB_CM_DREQ_SENT:
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		/* fall through */
	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
		cm_enter_timewait(cm_id_priv);
		break;
	case IB_CM_ESTABLISHED:
		if (cm_id_priv->id.lap_state == IB_CM_LAP_UNINIT ||
		    cm_id_priv->id.lap_state == IB_CM_LAP_SENT) {
			if (cm_id_priv->id.lap_state == IB_CM_LAP_SENT)
				ib_cancel_mad(cm_id_priv->av.port->mad_agent,
					      cm_id_priv->msg);
			cm_enter_timewait(cm_id_priv);
			break;
		}
		/* fall through */
	default:
		spin_unlock_irq(&cm_id_priv->lock);
		ret = -EINVAL;
		goto out;
	}

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
