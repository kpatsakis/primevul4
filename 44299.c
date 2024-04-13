static void cma_listen_on_dev(struct rdma_id_private *id_priv,
			      struct cma_device *cma_dev)
{
	struct rdma_id_private *dev_id_priv;
	struct rdma_cm_id *id;
	int ret;

	if (cma_family(id_priv) == AF_IB &&
	    rdma_node_get_transport(cma_dev->device->node_type) != RDMA_TRANSPORT_IB)
		return;

	id = rdma_create_id(cma_listen_handler, id_priv, id_priv->id.ps,
			    id_priv->id.qp_type);
	if (IS_ERR(id))
		return;

	dev_id_priv = container_of(id, struct rdma_id_private, id);

	dev_id_priv->state = RDMA_CM_ADDR_BOUND;
	memcpy(cma_src_addr(dev_id_priv), cma_src_addr(id_priv),
	       rdma_addr_size(cma_src_addr(id_priv)));

	cma_attach_to_dev(dev_id_priv, cma_dev);
	list_add_tail(&dev_id_priv->listen_list, &id_priv->listen_list);
	atomic_inc(&id_priv->refcount);
	dev_id_priv->internal_id = 1;
	dev_id_priv->afonly = id_priv->afonly;

	ret = rdma_listen(id, id_priv->backlog);
	if (ret)
		printk(KERN_WARNING "RDMA CMA: cma_listen_on_dev, error %d, "
		       "listening on device %s\n", ret, cma_dev->device->name);
}
