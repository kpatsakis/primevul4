static void cma_listen_on_all(struct rdma_id_private *id_priv)
{
	struct cma_device *cma_dev;

	mutex_lock(&lock);
	list_add_tail(&id_priv->list, &listen_any_list);
	list_for_each_entry(cma_dev, &dev_list, list)
		cma_listen_on_dev(id_priv, cma_dev);
	mutex_unlock(&lock);
}
