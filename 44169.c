static int cm_alloc_msg(struct cm_id_private *cm_id_priv,
			struct ib_mad_send_buf **msg)
{
	struct ib_mad_agent *mad_agent;
	struct ib_mad_send_buf *m;
	struct ib_ah *ah;

	mad_agent = cm_id_priv->av.port->mad_agent;
	ah = ib_create_ah(mad_agent->qp->pd, &cm_id_priv->av.ah_attr);
	if (IS_ERR(ah))
		return PTR_ERR(ah);

	m = ib_create_send_mad(mad_agent, cm_id_priv->id.remote_cm_qpn,
			       cm_id_priv->av.pkey_index,
			       0, IB_MGMT_MAD_HDR, IB_MGMT_MAD_DATA,
			       GFP_ATOMIC);
	if (IS_ERR(m)) {
		ib_destroy_ah(ah);
		return PTR_ERR(m);
	}

	/* Timeout set by caller if response is expected. */
	m->ah = ah;
	m->retries = cm_id_priv->max_cm_retries;

	atomic_inc(&cm_id_priv->refcount);
	m->context[0] = cm_id_priv;
	*msg = m;
	return 0;
}
