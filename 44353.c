int rdma_set_ib_paths(struct rdma_cm_id *id,
		      struct ib_sa_path_rec *path_rec, int num_paths)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_RESOLVED,
			   RDMA_CM_ROUTE_RESOLVED))
		return -EINVAL;

	id->route.path_rec = kmemdup(path_rec, sizeof *path_rec * num_paths,
				     GFP_KERNEL);
	if (!id->route.path_rec) {
		ret = -ENOMEM;
		goto err;
	}

	id->route.num_paths = num_paths;
	return 0;
err:
	cma_comp_exch(id_priv, RDMA_CM_ROUTE_RESOLVED, RDMA_CM_ADDR_RESOLVED);
	return ret;
}
