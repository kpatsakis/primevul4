static int cma_alloc_port(struct idr *ps, struct rdma_id_private *id_priv,
			  unsigned short snum)
{
	struct rdma_bind_list *bind_list;
	int ret;

	bind_list = kzalloc(sizeof *bind_list, GFP_KERNEL);
	if (!bind_list)
		return -ENOMEM;

	ret = idr_alloc(ps, bind_list, snum, snum + 1, GFP_KERNEL);
	if (ret < 0)
		goto err;

	bind_list->ps = ps;
	bind_list->port = (unsigned short)ret;
	cma_bind_port(bind_list, id_priv);
	return 0;
err:
	kfree(bind_list);
	return ret == -ENOSPC ? -EADDRNOTAVAIL : ret;
}
