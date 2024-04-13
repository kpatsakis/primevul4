struct ib_cm_id *ib_create_cm_id(struct ib_device *device,
				 ib_cm_handler cm_handler,
				 void *context)
{
	struct cm_id_private *cm_id_priv;
	int ret;

	cm_id_priv = kzalloc(sizeof *cm_id_priv, GFP_KERNEL);
	if (!cm_id_priv)
		return ERR_PTR(-ENOMEM);

	cm_id_priv->id.state = IB_CM_IDLE;
	cm_id_priv->id.device = device;
	cm_id_priv->id.cm_handler = cm_handler;
	cm_id_priv->id.context = context;
	cm_id_priv->id.remote_cm_qpn = 1;
	ret = cm_alloc_id(cm_id_priv);
	if (ret)
		goto error;

	spin_lock_init(&cm_id_priv->lock);
	init_completion(&cm_id_priv->comp);
	INIT_LIST_HEAD(&cm_id_priv->work_list);
	atomic_set(&cm_id_priv->work_count, -1);
	atomic_set(&cm_id_priv->refcount, 1);
	return &cm_id_priv->id;

error:
	kfree(cm_id_priv);
	return ERR_PTR(-ENOMEM);
}
