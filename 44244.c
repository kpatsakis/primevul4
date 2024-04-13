int ib_cm_listen(struct ib_cm_id *cm_id, __be64 service_id, __be64 service_mask,
		 struct ib_cm_compare_data *compare_data)
{
	struct cm_id_private *cm_id_priv, *cur_cm_id_priv;
	unsigned long flags;
	int ret = 0;

	service_mask = service_mask ? service_mask : ~cpu_to_be64(0);
	service_id &= service_mask;
	if ((service_id & IB_SERVICE_ID_AGN_MASK) == IB_CM_ASSIGN_SERVICE_ID &&
	    (service_id != IB_CM_ASSIGN_SERVICE_ID))
		return -EINVAL;

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	if (cm_id->state != IB_CM_IDLE)
		return -EINVAL;

	if (compare_data) {
		cm_id_priv->compare_data = kzalloc(sizeof *compare_data,
						   GFP_KERNEL);
		if (!cm_id_priv->compare_data)
			return -ENOMEM;
		cm_mask_copy(cm_id_priv->compare_data->data,
			     compare_data->data, compare_data->mask);
		memcpy(cm_id_priv->compare_data->mask, compare_data->mask,
		       IB_CM_COMPARE_SIZE);
	}

	cm_id->state = IB_CM_LISTEN;

	spin_lock_irqsave(&cm.lock, flags);
	if (service_id == IB_CM_ASSIGN_SERVICE_ID) {
		cm_id->service_id = cpu_to_be64(cm.listen_service_id++);
		cm_id->service_mask = ~cpu_to_be64(0);
	} else {
		cm_id->service_id = service_id;
		cm_id->service_mask = service_mask;
	}
	cur_cm_id_priv = cm_insert_listen(cm_id_priv);
	spin_unlock_irqrestore(&cm.lock, flags);

	if (cur_cm_id_priv) {
		cm_id->state = IB_CM_IDLE;
		kfree(cm_id_priv->compare_data);
		cm_id_priv->compare_data = NULL;
		ret = -EBUSY;
	}
	return ret;
}
