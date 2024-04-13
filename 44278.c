static void cma_deref_id(struct rdma_id_private *id_priv)
{
	if (atomic_dec_and_test(&id_priv->refcount))
		complete(&id_priv->comp);
}
