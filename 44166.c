static struct cm_id_private * cm_acquire_mraed_id(struct cm_mra_msg *mra_msg)
{
	switch (cm_mra_get_msg_mraed(mra_msg)) {
	case CM_MSG_RESPONSE_REQ:
		return cm_acquire_id(mra_msg->remote_comm_id, 0);
	case CM_MSG_RESPONSE_REP:
	case CM_MSG_RESPONSE_OTHER:
		return cm_acquire_id(mra_msg->remote_comm_id,
				     mra_msg->local_comm_id);
	default:
		return NULL;
	}
}
