tcp_send_queue (server *serv)
{
	char *buf, *p;
	int len, i, pri;
	GSList *list;
	time_t now = time (0);

	/* did the server close since the timeout was added? */
	if (!is_server (serv))
		return 0;

	/* try priority 2,1,0 */
	pri = 2;
	while (pri >= 0)
	{
		list = serv->outbound_queue;
		while (list)
		{
			buf = (char *) list->data;
			if (buf[0] == pri)
			{
				buf++;	/* skip the priority byte */
				len = strlen (buf);

				if (serv->next_send < now)
					serv->next_send = now;
				if (serv->next_send - now >= 10)
				{
					/* check for clock skew */
					if (now >= serv->prev_now)
						return 1;		  /* don't remove the timeout handler */
					/* it is skewed, reset to something sane */
					serv->next_send = now;
				}

				for (p = buf, i = len; i && *p != ' '; p++, i--);
				serv->next_send += (2 + i / 120);
				serv->sendq_len -= len;
				serv->prev_now = now;
				fe_set_throttle (serv);

				server_send_real (serv, buf, len);

				buf--;
				serv->outbound_queue = g_slist_remove (serv->outbound_queue, buf);
				free (buf);
				list = serv->outbound_queue;
			} else
			{
				list = list->next;
			}
		}
		/* now try pri 0 */
		pri--;
	}
	return 0;						  /* remove the timeout handler */
}
