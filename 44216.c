static struct cm_id_private * cm_insert_remote_sidr(struct cm_id_private
						    *cm_id_priv)
{
	struct rb_node **link = &cm.remote_sidr_table.rb_node;
	struct rb_node *parent = NULL;
	struct cm_id_private *cur_cm_id_priv;
	union ib_gid *port_gid = &cm_id_priv->av.dgid;
	__be32 remote_id = cm_id_priv->id.remote_id;

	while (*link) {
		parent = *link;
		cur_cm_id_priv = rb_entry(parent, struct cm_id_private,
					  sidr_id_node);
		if (be32_lt(remote_id, cur_cm_id_priv->id.remote_id))
			link = &(*link)->rb_left;
		else if (be32_gt(remote_id, cur_cm_id_priv->id.remote_id))
			link = &(*link)->rb_right;
		else {
			int cmp;
			cmp = memcmp(port_gid, &cur_cm_id_priv->av.dgid,
				     sizeof *port_gid);
			if (cmp < 0)
				link = &(*link)->rb_left;
			else if (cmp > 0)
				link = &(*link)->rb_right;
			else
				return cur_cm_id_priv;
		}
	}
	rb_link_node(&cm_id_priv->sidr_id_node, parent, link);
	rb_insert_color(&cm_id_priv->sidr_id_node, &cm.remote_sidr_table);
	return NULL;
}
