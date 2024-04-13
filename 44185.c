static int cm_establish(struct ib_cm_id *cm_id)
{
	struct cm_id_private *cm_id_priv;
	struct cm_work *work;
	unsigned long flags;
	int ret = 0;

	work = kmalloc(sizeof *work, GFP_ATOMIC);
	if (!work)
		return -ENOMEM;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	switch (cm_id->state)
	{
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
		cm_id->state = IB_CM_ESTABLISHED;
		break;
	case IB_CM_ESTABLISHED:
		ret = -EISCONN;
		break;
	default:
		ret = -EINVAL;
		break;
	}
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);

	if (ret) {
		kfree(work);
		goto out;
	}

	/*
	 * The CM worker thread may try to destroy the cm_id before it
	 * can execute this work item.  To prevent potential deadlock,
	 * we need to find the cm_id once we're in the context of the
	 * worker thread, rather than holding a reference on it.
	 */
	INIT_DELAYED_WORK(&work->work, cm_work_handler);
	work->local_id = cm_id->local_id;
	work->remote_id = cm_id->remote_id;
	work->mad_recv_wc = NULL;
	work->cm_event.event = IB_CM_USER_ESTABLISHED;
	queue_delayed_work(cm.wq, &work->work, 0);
out:
	return ret;
}
