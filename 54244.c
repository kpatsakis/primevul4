server_away_find_message (server *serv, char *nick)
{
	struct away_msg *away;
	GSList *list = away_list;
	while (list)
	{
		away = (struct away_msg *) list->data;
		if (away->server == serv && !serv->p_cmp (nick, away->nick))
			return away;
		list = list->next;
	}
	return NULL;
}
