static int cma_modify_qp_err(struct rdma_id_private *id_priv)
{
	struct ib_qp_attr qp_attr;
	int ret;

	mutex_lock(&id_priv->qp_mutex);
	if (!id_priv->id.qp) {
		ret = 0;
		goto out;
	}

	qp_attr.qp_state = IB_QPS_ERR;
	ret = ib_modify_qp(id_priv->id.qp, &qp_attr, IB_QP_STATE);
out:
	mutex_unlock(&id_priv->qp_mutex);
	return ret;
}
