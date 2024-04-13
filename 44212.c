static int cm_init_qp_rtr_attr(struct cm_id_private *cm_id_priv,
			       struct ib_qp_attr *qp_attr,
			       int *qp_attr_mask)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&cm_id_priv->lock, flags);
	switch (cm_id_priv->id.state) {
	case IB_CM_REQ_RCVD:
	case IB_CM_MRA_REQ_SENT:
	case IB_CM_REP_RCVD:
	case IB_CM_MRA_REP_SENT:
	case IB_CM_REP_SENT:
	case IB_CM_MRA_REP_RCVD:
	case IB_CM_ESTABLISHED:
		*qp_attr_mask = IB_QP_STATE | IB_QP_AV | IB_QP_PATH_MTU |
				IB_QP_DEST_QPN | IB_QP_RQ_PSN;
		qp_attr->ah_attr = cm_id_priv->av.ah_attr;
		if (!cm_id_priv->av.valid) {
			spin_unlock_irqrestore(&cm_id_priv->lock, flags);
			return -EINVAL;
		}
		if (cm_id_priv->av.ah_attr.vlan_id != 0xffff) {
			qp_attr->vlan_id = cm_id_priv->av.ah_attr.vlan_id;
			*qp_attr_mask |= IB_QP_VID;
		}
		if (!is_zero_ether_addr(cm_id_priv->av.smac)) {
			memcpy(qp_attr->smac, cm_id_priv->av.smac,
			       sizeof(qp_attr->smac));
			*qp_attr_mask |= IB_QP_SMAC;
		}
		if (cm_id_priv->alt_av.valid) {
			if (cm_id_priv->alt_av.ah_attr.vlan_id != 0xffff) {
				qp_attr->alt_vlan_id =
					cm_id_priv->alt_av.ah_attr.vlan_id;
				*qp_attr_mask |= IB_QP_ALT_VID;
			}
			if (!is_zero_ether_addr(cm_id_priv->alt_av.smac)) {
				memcpy(qp_attr->alt_smac,
				       cm_id_priv->alt_av.smac,
				       sizeof(qp_attr->alt_smac));
				*qp_attr_mask |= IB_QP_ALT_SMAC;
			}
		}
		qp_attr->path_mtu = cm_id_priv->path_mtu;
		qp_attr->dest_qp_num = be32_to_cpu(cm_id_priv->remote_qpn);
		qp_attr->rq_psn = be32_to_cpu(cm_id_priv->rq_psn);
		if (cm_id_priv->qp_type == IB_QPT_RC ||
		    cm_id_priv->qp_type == IB_QPT_XRC_TGT) {
			*qp_attr_mask |= IB_QP_MAX_DEST_RD_ATOMIC |
					 IB_QP_MIN_RNR_TIMER;
			qp_attr->max_dest_rd_atomic =
					cm_id_priv->responder_resources;
			qp_attr->min_rnr_timer = 0;
		}
		if (cm_id_priv->alt_av.ah_attr.dlid) {
			*qp_attr_mask |= IB_QP_ALT_PATH;
			qp_attr->alt_port_num = cm_id_priv->alt_av.port->port_num;
			qp_attr->alt_pkey_index = cm_id_priv->alt_av.pkey_index;
			qp_attr->alt_timeout = cm_id_priv->alt_av.timeout;
			qp_attr->alt_ah_attr = cm_id_priv->alt_av.ah_attr;
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
