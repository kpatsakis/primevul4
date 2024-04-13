static int cma_get_id_stats(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct nlmsghdr *nlh;
	struct rdma_cm_id_stats *id_stats;
	struct rdma_id_private *id_priv;
	struct rdma_cm_id *id = NULL;
	struct cma_device *cma_dev;
	int i_dev = 0, i_id = 0;

	/*
	 * We export all of the IDs as a sequence of messages.  Each
	 * ID gets its own netlink message.
	 */
	mutex_lock(&lock);

	list_for_each_entry(cma_dev, &dev_list, list) {
		if (i_dev < cb->args[0]) {
			i_dev++;
			continue;
		}

		i_id = 0;
		list_for_each_entry(id_priv, &cma_dev->id_list, list) {
			if (i_id < cb->args[1]) {
				i_id++;
				continue;
			}

			id_stats = ibnl_put_msg(skb, &nlh, cb->nlh->nlmsg_seq,
						sizeof *id_stats, RDMA_NL_RDMA_CM,
						RDMA_NL_RDMA_CM_ID_STATS);
			if (!id_stats)
				goto out;

			memset(id_stats, 0, sizeof *id_stats);
			id = &id_priv->id;
			id_stats->node_type = id->route.addr.dev_addr.dev_type;
			id_stats->port_num = id->port_num;
			id_stats->bound_dev_if =
				id->route.addr.dev_addr.bound_dev_if;

			if (ibnl_put_attr(skb, nlh,
					  rdma_addr_size(cma_src_addr(id_priv)),
					  cma_src_addr(id_priv),
					  RDMA_NL_RDMA_CM_ATTR_SRC_ADDR))
				goto out;
			if (ibnl_put_attr(skb, nlh,
					  rdma_addr_size(cma_src_addr(id_priv)),
					  cma_dst_addr(id_priv),
					  RDMA_NL_RDMA_CM_ATTR_DST_ADDR))
				goto out;

			id_stats->pid		= id_priv->owner;
			id_stats->port_space	= id->ps;
			id_stats->cm_state	= id_priv->state;
			id_stats->qp_num	= id_priv->qp_num;
			id_stats->qp_type	= id->qp_type;

			i_id++;
		}

		cb->args[1] = 0;
		i_dev++;
	}

out:
	mutex_unlock(&lock);
	cb->args[0] = i_dev;
	cb->args[1] = i_id;

	return skb->len;
}
