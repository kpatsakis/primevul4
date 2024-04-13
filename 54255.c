server_free (server *serv)
{
	serv->cleanup (serv);

	serv_list = g_slist_remove (serv_list, serv);

	dcc_notify_kill (serv);
	serv->flush_queue (serv);
	server_away_free_messages (serv);

	free (serv->nick_modes);
	free (serv->nick_prefixes);
	free (serv->chanmodes);
	free (serv->chantypes);
	if (serv->bad_nick_prefixes)
		free (serv->bad_nick_prefixes);
	if (serv->last_away_reason)
		free (serv->last_away_reason);
	if (serv->encoding)
		free (serv->encoding);
	if (serv->favlist)
		g_slist_free_full (serv->favlist, (GDestroyNotify) servlist_favchan_free);
#ifdef USE_OPENSSL
	if (serv->ctx)
		_SSL_context_free (serv->ctx);
#endif

	fe_server_callback (serv);

	free (serv);

	notify_cleanup ();
}
