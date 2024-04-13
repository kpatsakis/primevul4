server_get_network (server *serv, gboolean fallback)
{
	/* check the network list */
	if (serv->network)
		return ((ircnet *)serv->network)->name;

	/* check the network name given in 005 NETWORK=... */
	if (serv->server_session && *serv->server_session->channel)
		return serv->server_session->channel;

	if (fallback)
		return serv->servername;

	return NULL;
}
