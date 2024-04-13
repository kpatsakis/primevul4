static int cma_bind_loopback(struct rdma_id_private *id_priv)
{
	struct cma_device *cma_dev, *cur_dev;
	struct ib_port_attr port_attr;
	union ib_gid gid;
	u16 pkey;
	int ret;
	u8 p;

	cma_dev = NULL;
	mutex_lock(&lock);
	list_for_each_entry(cur_dev, &dev_list, list) {
		if (cma_family(id_priv) == AF_IB &&
		    rdma_node_get_transport(cur_dev->device->node_type) != RDMA_TRANSPORT_IB)
			continue;

		if (!cma_dev)
			cma_dev = cur_dev;

		for (p = 1; p <= cur_dev->device->phys_port_cnt; ++p) {
			if (!ib_query_port(cur_dev->device, p, &port_attr) &&
			    port_attr.state == IB_PORT_ACTIVE) {
				cma_dev = cur_dev;
				goto port_found;
			}
		}
	}

	if (!cma_dev) {
		ret = -ENODEV;
		goto out;
	}

	p = 1;

port_found:
	ret = ib_get_cached_gid(cma_dev->device, p, 0, &gid);
	if (ret)
		goto out;

	ret = ib_get_cached_pkey(cma_dev->device, p, 0, &pkey);
	if (ret)
		goto out;

	id_priv->id.route.addr.dev_addr.dev_type =
		(rdma_port_get_link_layer(cma_dev->device, p) == IB_LINK_LAYER_INFINIBAND) ?
		ARPHRD_INFINIBAND : ARPHRD_ETHER;

	rdma_addr_set_sgid(&id_priv->id.route.addr.dev_addr, &gid);
	ib_addr_set_pkey(&id_priv->id.route.addr.dev_addr, pkey);
	id_priv->id.port_num = p;
	cma_attach_to_dev(id_priv, cma_dev);
	cma_set_loopback(cma_src_addr(id_priv));
out:
	mutex_unlock(&lock);
	return ret;
}
