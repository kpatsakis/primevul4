static void dbus_bus_acquired(GDBusConnection *connection,
			      const gchar *name,
			      gpointer user_data)
{
	struct tcmur_handler **handler;

	tcmu_dbg("bus %s acquired\n", name);

	manager = g_dbus_object_manager_server_new("/org/kernel/TCMUService1");

	darray_foreach(handler, g_runner_handlers) {
		dbus_export_handler(*handler, G_CALLBACK(on_check_config));
	}

	dbus_handler_manager1_init(connection);
	g_dbus_object_manager_server_set_connection(manager, connection);
}
