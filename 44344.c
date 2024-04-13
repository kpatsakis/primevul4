int rdma_create_qp(struct rdma_cm_id *id, struct ib_pd *pd,
		   struct ib_qp_init_attr *qp_init_attr)
{
	struct rdma_id_private *id_priv;
	struct ib_qp *qp;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (id->device != pd->device)
		return -EINVAL;

	qp = ib_create_qp(pd, qp_init_attr);
	if (IS_ERR(qp))
		return PTR_ERR(qp);

	if (id->qp_type == IB_QPT_UD)
		ret = cma_init_ud_qp(id_priv, qp);
	else
		ret = cma_init_conn_qp(id_priv, qp);
	if (ret)
		goto err;

	id->qp = qp;
	id_priv->qp_num = qp->qp_num;
	id_priv->srq = (qp->srq != NULL);
	return 0;
err:
	ib_destroy_qp(qp);
	return ret;
}
