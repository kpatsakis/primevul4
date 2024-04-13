advertise_sasl_exit(hook_data_client_exit *data)
{
	if (!ConfigFileEntry.sasl_service)
		return;

	if (irccmp(data->target->name, ConfigFileEntry.sasl_service))
		return;

	sendto_local_clients_with_capability(CLICAP_CAP_NOTIFY, ":%s CAP * DEL :sasl", me.name);
}
