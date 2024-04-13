server_away_free_messages (server *serv)
{
	GSList *list, *next;
	struct away_msg *away;

	list = away_list;
	while (list)
	{
		away = list->data;
		next = list->next;
		if (away->server == serv)
		{
			away_list = g_slist_remove (away_list, away);
			if (away->message)
				free (away->message);
			free (away);
			next = away_list;
		}
		list = next;
	}
}
