on_dbus_check_config(TCMUService1 *interface,
		     GDBusMethodInvocation *invocation,
		     gchar *cfgstring,
		     gpointer user_data)
{
	char *bus_name, *obj_name;
	struct tcmur_handler *handler = user_data;
	GDBusConnection *connection;
	GError *error = NULL;
	GVariant *result;

	bus_name = g_strdup_printf("org.kernel.TCMUService1.HandlerManager1.%s",
				   handler->subtype);
	obj_name = g_strdup_printf("/org/kernel/TCMUService1/HandlerManager1/%s",
				   handler->subtype);
	connection = g_dbus_method_invocation_get_connection(invocation);
	result = g_dbus_connection_call_sync(connection,
					     bus_name,
					     obj_name,
					     "org.kernel.TCMUService1",
					     "CheckConfig",
					     g_variant_new("(s)", cfgstring),
					     NULL, G_DBUS_CALL_FLAGS_NONE, -1,
					     NULL, &error);
	if (result)
		g_dbus_method_invocation_return_value(invocation, result);
	else
		g_dbus_method_invocation_return_value(invocation,
			g_variant_new("(bs)", FALSE, error->message));
	g_free(bus_name);
	g_free(obj_name);
	return TRUE;
}
