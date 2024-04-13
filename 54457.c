on_check_config(TCMUService1 *interface,
		GDBusMethodInvocation *invocation,
		gchar *cfgstring,
		gpointer user_data)
{
	struct tcmur_handler *handler = user_data;
	char *reason = NULL;
	bool str_ok = true;

	if (handler->check_config)
		str_ok = handler->check_config(cfgstring, &reason);

	if (str_ok)
		reason = "success";

	g_dbus_method_invocation_return_value(invocation,
		    g_variant_new("(bs)", str_ok, reason ? : "unknown"));

	if (!str_ok)
		free(reason);

	return TRUE;
}
