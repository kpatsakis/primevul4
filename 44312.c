static void cma_release_dev(struct rdma_id_private *id_priv)
{
	mutex_lock(&lock);
	list_del(&id_priv->list);
	cma_deref_dev(id_priv->cma_dev);
	id_priv->cma_dev = NULL;
	mutex_unlock(&lock);
}
