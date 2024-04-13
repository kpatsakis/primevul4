static void cma_cancel_route(struct rdma_id_private *id_priv)
{
	switch (rdma_port_get_link_layer(id_priv->id.device, id_priv->id.port_num)) {
	case IB_LINK_LAYER_INFINIBAND:
		if (id_priv->query)
			ib_sa_cancel_query(id_priv->query_id, id_priv->query);
		break;
	default:
		break;
	}
}
