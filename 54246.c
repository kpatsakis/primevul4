server_away_save_message (server *serv, char *nick, char *msg)
{
	struct away_msg *away = server_away_find_message (serv, nick);

	if (away)						  /* Change message for known user */
	{
		if (away->message)
			free (away->message);
		away->message = strdup (msg);
	} else
		/* Create brand new entry */
	{
		away = malloc (sizeof (struct away_msg));
		if (away)
		{
			away->server = serv;
			safe_strcpy (away->nick, nick, sizeof (away->nick));
			away->message = strdup (msg);
			away_list = g_slist_prepend (away_list, away);
		}
	}
}
