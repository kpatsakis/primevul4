static struct cm_id_private * cm_get_id(__be32 local_id, __be32 remote_id)
{
	struct cm_id_private *cm_id_priv;

	cm_id_priv = idr_find(&cm.local_id_table,
			      (__force int) (local_id ^ cm.random_id_operand));
	if (cm_id_priv) {
		if (cm_id_priv->id.remote_id == remote_id)
			atomic_inc(&cm_id_priv->refcount);
		else
			cm_id_priv = NULL;
	}

	return cm_id_priv;
}
