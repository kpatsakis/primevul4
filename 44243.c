int ib_cm_init_qp_attr(struct ib_cm_id *cm_id,
		       struct ib_qp_attr *qp_attr,
		       int *qp_attr_mask)
{
	struct cm_id_private *cm_id_priv;
	int ret;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	switch (qp_attr->qp_state) {
	case IB_QPS_INIT:
		ret = cm_init_qp_init_attr(cm_id_priv, qp_attr, qp_attr_mask);
		break;
	case IB_QPS_RTR:
		ret = cm_init_qp_rtr_attr(cm_id_priv, qp_attr, qp_attr_mask);
		break;
	case IB_QPS_RTS:
		ret = cm_init_qp_rts_attr(cm_id_priv, qp_attr, qp_attr_mask);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}
