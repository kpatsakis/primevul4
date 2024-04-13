 static int cm_init_av_by_path(struct ib_sa_path_rec *path, struct cm_av *av)
 {
 	struct cm_device *cm_dev;
	struct cm_port *port = NULL;
	unsigned long flags;
	int ret;
	u8 p;

	read_lock_irqsave(&cm.device_lock, flags);
	list_for_each_entry(cm_dev, &cm.device_list, list) {
		if (!ib_find_cached_gid(cm_dev->ib_device, &path->sgid,
					&p, NULL)) {
			port = cm_dev->port[p-1];
			break;
		}
	}
	read_unlock_irqrestore(&cm.device_lock, flags);

	if (!port)
		return -EINVAL;

	ret = ib_find_cached_pkey(cm_dev->ib_device, port->port_num,
				  be16_to_cpu(path->pkey), &av->pkey_index);
	if (ret)
		return ret;

	av->port = port;
	ib_init_ah_from_path(cm_dev->ib_device, port->port_num, path,
			     &av->ah_attr);
	av->timeout = path->packet_life_time + 1;
	memcpy(av->smac, path->smac, sizeof(av->smac));

	av->valid = 1;
	return 0;
}
