static int cm_init_qp_rts_attr(struct cm_id_private *cm_id_priv,
			       struct ib_qp_attr *qp_attr,
			       int *qp_attr_mask)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&cm_id_priv->lock, flags);
	switch (cm_id_priv->id.state) {
	/* Allow transition to RTS before sending REP */
	case IB_CM_REQ_RCVD:
	case IB_CM_MRA_REQ_SENT:

	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
	case IB_CM_ESTABLISHED:
		if (cm_id_priv->id.lap_state == IB_CM_LAP_UNINIT) {
			*qp_attr_mask = IB_QP_STATE | IB_QP_SQ_PSN;
			qp_attr->sq_psn = be32_to_cpu(cm_id_priv->sq_psn);
			switch (cm_id_priv->qp_type) {
			case IB_QPT_RC:
			case IB_QPT_XRC_INI:
				*qp_attr_mask |= IB_QP_RETRY_CNT | IB_QP_RNR_RETRY |
						 IB_QP_MAX_QP_RD_ATOMIC;
				qp_attr->retry_cnt = cm_id_priv->retry_count;
				qp_attr->rnr_retry = cm_id_priv->rnr_retry_count;
				qp_attr->max_rd_atomic = cm_id_priv->initiator_depth;
				/* fall through */
			case IB_QPT_XRC_TGT:
				*qp_attr_mask |= IB_QP_TIMEOUT;
				qp_attr->timeout = cm_id_priv->av.timeout;
				break;
			default:
				break;
			}
			if (cm_id_priv->alt_av.ah_attr.dlid) {
				*qp_attr_mask |= IB_QP_PATH_MIG_STATE;
				qp_attr->path_mig_state = IB_MIG_REARM;
			}
		} else {
			*qp_attr_mask = IB_QP_ALT_PATH | IB_QP_PATH_MIG_STATE;
			qp_attr->alt_port_num = cm_id_priv->alt_av.port->port_num;
			qp_attr->alt_pkey_index = cm_id_priv->alt_av.pkey_index;
			qp_attr->alt_timeout = cm_id_priv->alt_av.timeout;
			qp_attr->alt_ah_attr = cm_id_priv->alt_av.ah_attr;
			qp_attr->path_mig_state = IB_MIG_REARM;
		}
		ret = 0;
		break;
	default:
		ret = -EINVAL;
		break;
	}
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return ret;
}
