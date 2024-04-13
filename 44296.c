static void cma_leave_mc_groups(struct rdma_id_private *id_priv)
{
	struct cma_multicast *mc;

	while (!list_empty(&id_priv->mc_list)) {
		mc = container_of(id_priv->mc_list.next,
				  struct cma_multicast, list);
		list_del(&mc->list);
		switch (rdma_port_get_link_layer(id_priv->cma_dev->device, id_priv->id.port_num)) {
		case IB_LINK_LAYER_INFINIBAND:
			ib_sa_free_multicast(mc->multicast.ib);
			kfree(mc);
			break;
		case IB_LINK_LAYER_ETHERNET:
			kref_put(&mc->mcref, release_mc);
			break;
		default:
			break;
		}
	}
}
