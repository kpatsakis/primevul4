void dbus_handler_manager1_init(GDBusConnection *connection)
{
	GError *error = NULL;
	TCMUService1HandlerManager1 *interface;
	gboolean ret;

	interface = tcmuservice1_handler_manager1_skeleton_new();
	ret = g_dbus_interface_skeleton_export(
			G_DBUS_INTERFACE_SKELETON(interface),
			connection,
			"/org/kernel/TCMUService1/HandlerManager1",
			&error);
	g_signal_connect(interface,
			 "handle-register-handler",
			 G_CALLBACK (on_register_handler),
			 NULL);
	g_signal_connect(interface,
			 "handle-unregister-handler",
			 G_CALLBACK (on_unregister_handler),
			 NULL);
	if (!ret)
		tcmu_err("Handler manager export failed: %s\n",
		     error ? error->message : "unknown error");
	if (error)
		g_error_free(error);
}
