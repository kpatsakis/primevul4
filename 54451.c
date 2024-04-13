static void dbus_name_lost(GDBusConnection *connection,
			   const gchar *name,
			   gpointer user_data)
{
	tcmu_dbg("name lost\n");
}
