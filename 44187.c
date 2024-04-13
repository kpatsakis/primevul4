static struct cm_id_private * cm_find_listen(struct ib_device *device,
					     __be64 service_id,
					     u8 *private_data)
{
	struct rb_node *node = cm.listen_service_table.rb_node;
	struct cm_id_private *cm_id_priv;
	int data_cmp;

	while (node) {
		cm_id_priv = rb_entry(node, struct cm_id_private, service_node);
		data_cmp = cm_compare_private_data(private_data,
						   cm_id_priv->compare_data);
		if ((cm_id_priv->id.service_mask & service_id) ==
		     cm_id_priv->id.service_id &&
		    (cm_id_priv->id.device == device) && !data_cmp)
			return cm_id_priv;

		if (device < cm_id_priv->id.device)
			node = node->rb_left;
		else if (device > cm_id_priv->id.device)
			node = node->rb_right;
		else if (be64_lt(service_id, cm_id_priv->id.service_id))
			node = node->rb_left;
		else if (be64_gt(service_id, cm_id_priv->id.service_id))
			node = node->rb_right;
		else if (data_cmp < 0)
			node = node->rb_left;
		else
			node = node->rb_right;
	}
	return NULL;
}
