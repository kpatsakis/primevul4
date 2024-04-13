static struct cm_id_private * cm_acquire_rejected_id(struct cm_rej_msg *rej_msg)
{
	struct cm_timewait_info *timewait_info;
	struct cm_id_private *cm_id_priv;
	__be32 remote_id;

	remote_id = rej_msg->local_comm_id;

	if (__be16_to_cpu(rej_msg->reason) == IB_CM_REJ_TIMEOUT) {
		spin_lock_irq(&cm.lock);
		timewait_info = cm_find_remote_id( *((__be64 *) rej_msg->ari),
						  remote_id);
		if (!timewait_info) {
			spin_unlock_irq(&cm.lock);
			return NULL;
		}
		cm_id_priv = idr_find(&cm.local_id_table, (__force int)
				      (timewait_info->work.local_id ^
				       cm.random_id_operand));
		if (cm_id_priv) {
			if (cm_id_priv->id.remote_id == remote_id)
				atomic_inc(&cm_id_priv->refcount);
			else
				cm_id_priv = NULL;
		}
		spin_unlock_irq(&cm.lock);
	} else if (cm_rej_get_msg_rejected(rej_msg) == CM_MSG_RESPONSE_REQ)
		cm_id_priv = cm_acquire_id(rej_msg->remote_comm_id, 0);
	else
		cm_id_priv = cm_acquire_id(rej_msg->remote_comm_id, remote_id);

	return cm_id_priv;
}
