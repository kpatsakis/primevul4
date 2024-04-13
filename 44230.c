static void cm_remove_port_fs(struct cm_port *port)
{
	int i;

	for (i = 0; i < CM_COUNTER_GROUPS; i++)
		kobject_put(&port->counter_group[i].obj);

	kobject_put(&port->port_obj);
}
