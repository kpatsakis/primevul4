static int cm_create_port_fs(struct cm_port *port)
{
	int i, ret;

	ret = kobject_init_and_add(&port->port_obj, &cm_port_obj_type,
				   &port->cm_dev->device->kobj,
				   "%d", port->port_num);
	if (ret) {
		kfree(port);
		return ret;
	}

	for (i = 0; i < CM_COUNTER_GROUPS; i++) {
		ret = kobject_init_and_add(&port->counter_group[i].obj,
					   &cm_counter_obj_type,
					   &port->port_obj,
					   "%s", counter_group_names[i]);
		if (ret)
			goto error;
	}

	return 0;

error:
	while (i--)
		kobject_put(&port->counter_group[i].obj);
	kobject_put(&port->port_obj);
	return ret;

}
