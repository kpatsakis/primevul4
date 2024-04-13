static void cm_reset_to_idle(struct cm_id_private *cm_id_priv)
{
	unsigned long flags;

	cm_id_priv->id.state = IB_CM_IDLE;
	if (cm_id_priv->timewait_info) {
		spin_lock_irqsave(&cm.lock, flags);
		cm_cleanup_timewait(cm_id_priv->timewait_info);
		spin_unlock_irqrestore(&cm.lock, flags);
		kfree(cm_id_priv->timewait_info);
		cm_id_priv->timewait_info = NULL;
	}
}
