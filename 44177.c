static inline void cm_deref_id(struct cm_id_private *cm_id_priv)
{
	if (atomic_dec_and_test(&cm_id_priv->refcount))
		complete(&cm_id_priv->comp);
}
