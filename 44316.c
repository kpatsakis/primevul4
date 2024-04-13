static int cma_rep_recv(struct rdma_id_private *id_priv)
{
	int ret;

	ret = cma_modify_qp_rtr(id_priv, NULL);
	if (ret)
		goto reject;

	ret = cma_modify_qp_rts(id_priv, NULL);
	if (ret)
		goto reject;

	ret = ib_send_cm_rtu(id_priv->cm_id.ib, NULL, 0);
	if (ret)
		goto reject;

	return 0;
reject:
	cma_modify_qp_err(id_priv);
	ib_send_cm_rej(id_priv->cm_id.ib, IB_CM_REJ_CONSUMER_DEFINED,
		       NULL, 0, NULL, 0);
	return ret;
}
