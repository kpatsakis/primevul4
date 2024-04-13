server_read_child (GIOChannel *source, GIOCondition condition, server *serv)
{
	session *sess = serv->server_session;
	char tbuf[128];
	char outbuf[512];
	char host[100];
	char ip[100];
#ifdef USE_MSPROXY
	char *p;
#endif

	waitline2 (source, tbuf, sizeof tbuf);

	switch (tbuf[0])
	{
	case '0':	/* print some text */
		waitline2 (source, tbuf, sizeof tbuf);
		PrintText (serv->server_session, tbuf);
		break;
	case '1':						  /* unknown host */
		server_stopconnecting (serv);
		closesocket (serv->sok4);
		if (serv->proxy_sok4 != -1)
			closesocket (serv->proxy_sok4);
#ifdef USE_IPV6
		if (serv->sok6 != -1)
			closesocket (serv->sok6);
		if (serv->proxy_sok6 != -1)
			closesocket (serv->proxy_sok6);
#endif
		EMIT_SIGNAL (XP_TE_UKNHOST, sess, NULL, NULL, NULL, NULL, 0);
		if (!servlist_cycle (serv))
			if (prefs.hex_net_auto_reconnectonfail)
				auto_reconnect (serv, FALSE, -1);
		break;
	case '2':						  /* connection failed */
		waitline2 (source, tbuf, sizeof tbuf);
		server_stopconnecting (serv);
		closesocket (serv->sok4);
		if (serv->proxy_sok4 != -1)
			closesocket (serv->proxy_sok4);
#ifdef USE_IPV6
		if (serv->sok6 != -1)
			closesocket (serv->sok6);
		if (serv->proxy_sok6 != -1)
			closesocket (serv->proxy_sok6);
#endif
		EMIT_SIGNAL (XP_TE_CONNFAIL, sess, errorstring (atoi (tbuf)), NULL,
						 NULL, NULL, 0);
		if (!servlist_cycle (serv))
			if (prefs.hex_net_auto_reconnectonfail)
				auto_reconnect (serv, FALSE, -1);
		break;
	case '3':						  /* gethostbyname finished */
		waitline2 (source, host, sizeof host);
		waitline2 (source, ip, sizeof ip);
		waitline2 (source, outbuf, sizeof outbuf);
		EMIT_SIGNAL (XP_TE_CONNECT, sess, host, ip, outbuf, NULL, 0);
#ifdef WIN32
		if (prefs.hex_identd)
		{
			if (serv->network && ((ircnet *)serv->network)->user)
			{
				identd_start (((ircnet *)serv->network)->user);
			}
			else
			{
				identd_start (prefs.hex_irc_user_name);
			}
		}
#else
		snprintf (outbuf, sizeof (outbuf), "%s/auth/xchat_auth",
					 g_get_home_dir ());
		if (access (outbuf, X_OK) == 0)
		{
			snprintf (outbuf, sizeof (outbuf), "exec -d %s/auth/xchat_auth %s",
						 g_get_home_dir (), prefs.hex_irc_user_name);
			handle_command (serv->server_session, outbuf, FALSE);
		}
#endif
		break;
	case '4':						  /* success */
		waitline2 (source, tbuf, sizeof (tbuf));
#ifdef USE_MSPROXY
		serv->sok = strtol (tbuf, &p, 10);
		if (*p++ == ' ')
		{
			serv->proxy_sok = strtol (p, &p, 10);
			serv->msp_state.clientid = strtol (++p, &p, 10);
			serv->msp_state.serverid = strtol (++p, &p, 10);
			serv->msp_state.seq_sent = atoi (++p);
		} else
			serv->proxy_sok = -1;
#ifdef DEBUG_MSPROXY
		printf ("Parent got main socket: %d, proxy socket: %d\n", serv->sok, serv->proxy_sok);
		printf ("Client ID 0x%08x server ID 0x%08x seq_sent %d\n", serv->msp_state.clientid, serv->msp_state.serverid, serv->msp_state.seq_sent);
#endif
#else
		serv->sok = atoi (tbuf);
#endif
#ifdef USE_IPV6
		/* close the one we didn't end up using */
		if (serv->sok == serv->sok4)
			closesocket (serv->sok6);
		else
			closesocket (serv->sok4);
		if (serv->proxy_sok != -1)
		{
			if (serv->proxy_sok == serv->proxy_sok4)
				closesocket (serv->proxy_sok6);
			else
				closesocket (serv->proxy_sok4);
		}
#endif
		server_connect_success (serv);
		break;
	case '5':						  /* prefs ip discovered */
		waitline2 (source, tbuf, sizeof tbuf);
		prefs.local_ip = inet_addr (tbuf);
		break;
	case '7':						  /* gethostbyname (prefs.hex_net_bind_host) failed */
		sprintf (outbuf,
					_("Cannot resolve hostname %s\nCheck your IP Settings!\n"),
					prefs.hex_net_bind_host);
		PrintText (sess, outbuf);
		break;
	case '8':
		PrintText (sess, _("Proxy traversal failed.\n"));
		server_disconnect (sess, FALSE, -1);
		break;
	case '9':
		waitline2 (source, tbuf, sizeof tbuf);
		EMIT_SIGNAL (XP_TE_SERVERLOOKUP, sess, tbuf, NULL, NULL, NULL, 0);
		break;
	}

	return TRUE;
}
