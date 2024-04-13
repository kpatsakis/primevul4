static struct cm_id_private * cm_insert_listen(struct cm_id_private *cm_id_priv)
{
	struct rb_node **link = &cm.listen_service_table.rb_node;
	struct rb_node *parent = NULL;
	struct cm_id_private *cur_cm_id_priv;
	__be64 service_id = cm_id_priv->id.service_id;
	__be64 service_mask = cm_id_priv->id.service_mask;
	int data_cmp;

	while (*link) {
		parent = *link;
		cur_cm_id_priv = rb_entry(parent, struct cm_id_private,
					  service_node);
		data_cmp = cm_compare_data(cm_id_priv->compare_data,
					   cur_cm_id_priv->compare_data);
		if ((cur_cm_id_priv->id.service_mask & service_id) ==
		    (service_mask & cur_cm_id_priv->id.service_id) &&
		    (cm_id_priv->id.device == cur_cm_id_priv->id.device) &&
		    !data_cmp)
			return cur_cm_id_priv;

		if (cm_id_priv->id.device < cur_cm_id_priv->id.device)
			link = &(*link)->rb_left;
		else if (cm_id_priv->id.device > cur_cm_id_priv->id.device)
			link = &(*link)->rb_right;
		else if (be64_lt(service_id, cur_cm_id_priv->id.service_id))
			link = &(*link)->rb_left;
		else if (be64_gt(service_id, cur_cm_id_priv->id.service_id))
			link = &(*link)->rb_right;
		else if (data_cmp < 0)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}
	rb_link_node(&cm_id_priv->service_node, parent, link);
	rb_insert_color(&cm_id_priv->service_node, &cm.listen_service_table);
	return NULL;
}
