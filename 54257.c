server_new (void)
{
	static int id = 0;
	server *serv;

	serv = malloc (sizeof (struct server));
	memset (serv, 0, sizeof (struct server));

	/* use server.c and proto-irc.c functions */
	server_fill_her_up (serv);

	serv->id = id++;
	serv->sok = -1;
	strcpy (serv->nick, prefs.hex_irc_nick1);
	server_set_defaults (serv);

	serv_list = g_slist_prepend (serv_list, serv);

	fe_new_server (serv);

	return serv;
}
