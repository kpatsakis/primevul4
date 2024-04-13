static void cm_destroy_id(struct ib_cm_id *cm_id, int err)
{
	struct cm_id_private *cm_id_priv;
	struct cm_work *work;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
retest:
	spin_lock_irq(&cm_id_priv->lock);
	switch (cm_id->state) {
	case IB_CM_LISTEN:
		cm_id->state = IB_CM_IDLE;
		spin_unlock_irq(&cm_id_priv->lock);
		spin_lock_irq(&cm.lock);
		rb_erase(&cm_id_priv->service_node, &cm.listen_service_table);
		spin_unlock_irq(&cm.lock);
		break;
	case IB_CM_SIDR_REQ_SENT:
		cm_id->state = IB_CM_IDLE;
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		spin_unlock_irq(&cm_id_priv->lock);
		break;
	case IB_CM_SIDR_REQ_RCVD:
		spin_unlock_irq(&cm_id_priv->lock);
		cm_reject_sidr_req(cm_id_priv, IB_SIDR_REJECT);
		break;
	case IB_CM_REQ_SENT:
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		spin_unlock_irq(&cm_id_priv->lock);
		ib_send_cm_rej(cm_id, IB_CM_REJ_TIMEOUT,
			       &cm_id_priv->id.device->node_guid,
			       sizeof cm_id_priv->id.device->node_guid,
			       NULL, 0);
		break;
	case IB_CM_REQ_RCVD:
		if (err == -ENOMEM) {
			/* Do not reject to allow future retries. */
			cm_reset_to_idle(cm_id_priv);
			spin_unlock_irq(&cm_id_priv->lock);
		} else {
			spin_unlock_irq(&cm_id_priv->lock);
			ib_send_cm_rej(cm_id, IB_CM_REJ_CONSUMER_DEFINED,
				       NULL, 0, NULL, 0);
		}
		break;
	case IB_CM_MRA_REQ_RCVD:
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		/* Fall through */
	case IB_CM_MRA_REQ_SENT:
	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
		spin_unlock_irq(&cm_id_priv->lock);
		ib_send_cm_rej(cm_id, IB_CM_REJ_CONSUMER_DEFINED,
			       NULL, 0, NULL, 0);
		break;
	case IB_CM_ESTABLISHED:
		spin_unlock_irq(&cm_id_priv->lock);
		if (cm_id_priv->qp_type == IB_QPT_XRC_TGT)
			break;
		ib_send_cm_dreq(cm_id, NULL, 0);
		goto retest;
	case IB_CM_DREQ_SENT:
		ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
		cm_enter_timewait(cm_id_priv);
		spin_unlock_irq(&cm_id_priv->lock);
		break;
	case IB_CM_DREQ_RCVD:
		spin_unlock_irq(&cm_id_priv->lock);
		ib_send_cm_drep(cm_id, NULL, 0);
		break;
	default:
		spin_unlock_irq(&cm_id_priv->lock);
		break;
	}

	cm_free_id(cm_id->local_id);
	cm_deref_id(cm_id_priv);
	wait_for_completion(&cm_id_priv->comp);
	while ((work = cm_dequeue_work(cm_id_priv)) != NULL)
		cm_free_work(work);
	kfree(cm_id_priv->compare_data);
	kfree(cm_id_priv->private_data);
	kfree(cm_id_priv);
}
