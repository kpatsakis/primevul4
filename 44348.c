void rdma_leave_multicast(struct rdma_cm_id *id, struct sockaddr *addr)
{
	struct rdma_id_private *id_priv;
	struct cma_multicast *mc;

	id_priv = container_of(id, struct rdma_id_private, id);
	spin_lock_irq(&id_priv->lock);
	list_for_each_entry(mc, &id_priv->mc_list, list) {
		if (!memcmp(&mc->addr, addr, rdma_addr_size(addr))) {
			list_del(&mc->list);
			spin_unlock_irq(&id_priv->lock);

			if (id->qp)
				ib_detach_mcast(id->qp,
						&mc->multicast.ib->rec.mgid,
						be16_to_cpu(mc->multicast.ib->rec.mlid));
			if (rdma_node_get_transport(id_priv->cma_dev->device->node_type) == RDMA_TRANSPORT_IB) {
				switch (rdma_port_get_link_layer(id->device, id->port_num)) {
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
			return;
		}
	}
	spin_unlock_irq(&id_priv->lock);
}
