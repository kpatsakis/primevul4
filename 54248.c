server_cleanup (server * serv)
{
	fe_set_lag (serv, 0);

	if (serv->iotag)
	{
		fe_input_remove (serv->iotag);
		serv->iotag = 0;
	}

	if (serv->joindelay_tag)
	{
		fe_timeout_remove (serv->joindelay_tag);
		serv->joindelay_tag = 0;
	}

#ifdef USE_OPENSSL
	if (serv->ssl)
	{
		SSL_shutdown (serv->ssl);
		SSL_free (serv->ssl);
		serv->ssl = NULL;
	}
#endif

	if (serv->connecting)
	{
		server_stopconnecting (serv);
		closesocket (serv->sok4);
		if (serv->proxy_sok4 != -1)
			closesocket (serv->proxy_sok4);
		if (serv->sok6 != -1)
			closesocket (serv->sok6);
		if (serv->proxy_sok6 != -1)
			closesocket (serv->proxy_sok6);
		return 1;
	}

	if (serv->connected)
	{
		close_socket (serv->sok);
		if (serv->proxy_sok)
			close_socket (serv->proxy_sok);
		serv->connected = FALSE;
		serv->end_of_motd = FALSE;
		return 2;
	}

	/* is this server in a reconnect delay? remove it! */
	if (serv->recondelay_tag)
	{
		fe_timeout_remove (serv->recondelay_tag);
		serv->recondelay_tag = 0;
		return 3;
	}

	return 0;
}
