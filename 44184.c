static void cm_enter_timewait(struct cm_id_private *cm_id_priv)
{
	int wait_time;
	unsigned long flags;

	spin_lock_irqsave(&cm.lock, flags);
	cm_cleanup_timewait(cm_id_priv->timewait_info);
	list_add_tail(&cm_id_priv->timewait_info->list, &cm.timewait_list);
	spin_unlock_irqrestore(&cm.lock, flags);

	/*
	 * The cm_id could be destroyed by the user before we exit timewait.
	 * To protect against this, we search for the cm_id after exiting
	 * timewait before notifying the user that we've exited timewait.
	 */
	cm_id_priv->id.state = IB_CM_TIMEWAIT;
	wait_time = cm_convert_to_ms(cm_id_priv->av.timeout);
	queue_delayed_work(cm.wq, &cm_id_priv->timewait_info->work.work,
			   msecs_to_jiffies(wait_time));
	cm_id_priv->timewait_info = NULL;
}
