server_disconnect (session * sess, int sendquit, int err)
{
	server *serv = sess->server;
	GSList *list;
	char tbuf[64];
	gboolean shutup = FALSE;

	/* send our QUIT reason */
	if (sendquit && serv->connected)
	{
		server_sendquit (sess);
	}

	fe_server_event (serv, FE_SE_DISCONNECT, 0);

	/* close all sockets & io tags */
	switch (server_cleanup (serv))
	{
	case 0:							  /* it wasn't even connected! */
		notc_msg (sess);
		return;
	case 1:							  /* it was in the process of connecting */
		sprintf (tbuf, "%d", sess->server->childpid);
		EMIT_SIGNAL (XP_TE_STOPCONNECT, sess, tbuf, NULL, NULL, NULL, 0);
		return;
	case 3:
		shutup = TRUE;	/* won't print "disconnected" in channels */
	}

	server_flush_queue (serv);

	list = sess_list;
	while (list)
	{
		sess = (struct session *) list->data;
		if (sess->server == serv)
		{
			if (!shutup || sess->type == SESS_SERVER)
				/* print "Disconnected" to each window using this server */
				EMIT_SIGNAL (XP_TE_DISCON, sess, errorstring (err), NULL, NULL, NULL, 0);

			if (!sess->channel[0] || sess->type == SESS_CHANNEL)
				clear_channel (sess);
		}
		list = list->next;
	}

	serv->pos = 0;
	serv->motd_skipped = FALSE;
	serv->no_login = FALSE;
	serv->servername[0] = 0;
	serv->lag_sent = 0;

	notify_cleanup ();
}
