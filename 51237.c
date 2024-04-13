advertise_sasl(struct Client *client_p)
{
	if (!ConfigFileEntry.sasl_service)
		return;

	if (irccmp(client_p->name, ConfigFileEntry.sasl_service))
		return;

	sendto_local_clients_with_capability(CLICAP_CAP_NOTIFY, ":%s CAP * NEW :sasl", me.name);
}
