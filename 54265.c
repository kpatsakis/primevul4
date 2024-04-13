tcp_send_len (server *serv, char *buf, int len)
{
	char *dbuf;
	int noqueue = !serv->outbound_queue;

	if (!prefs.hex_net_throttle)
		return server_send_real (serv, buf, len);

	dbuf = malloc (len + 2);	/* first byte is the priority */
	dbuf[0] = 2;	/* pri 2 for most things */
	memcpy (dbuf + 1, buf, len);
	dbuf[len + 1] = 0;

	/* privmsg and notice get a lower priority */
	if (g_ascii_strncasecmp (dbuf + 1, "PRIVMSG", 7) == 0 ||
		 g_ascii_strncasecmp (dbuf + 1, "NOTICE", 6) == 0)
	{
		dbuf[0] = 1;
	}
	else
	{
		/* WHO/MODE get the lowest priority */
		if (g_ascii_strncasecmp (dbuf + 1, "WHO ", 4) == 0 ||
		/* but only MODE queries, not changes */
			(g_ascii_strncasecmp (dbuf + 1, "MODE", 4) == 0 &&
			 strchr (dbuf, '-') == NULL &&
			 strchr (dbuf, '+') == NULL))
			dbuf[0] = 0;
	}

	serv->outbound_queue = g_slist_append (serv->outbound_queue, dbuf);
	serv->sendq_len += len; /* tcp_send_queue uses strlen */

	if (tcp_send_queue (serv) && noqueue)
		fe_timeout_add (500, tcp_send_queue, serv);

	return 1;
}
