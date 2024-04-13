void serveconnection(CLIENT *client) {
	if (client->server->transactionlog && (client->transactionlogfd == -1))
	{
		if (-1 == (client->transactionlogfd = open(client->server->transactionlog,
							   O_WRONLY | O_CREAT,
							   S_IRUSR | S_IWUSR)))
			g_warning("Could not open transaction log %s",
				  client->server->transactionlog);
	}

	if(do_run(client->server->prerun, client->exportname)) {
		exit(EXIT_FAILURE);
	}
	setupexport(client);

	if (client->server->flags & F_COPYONWRITE) {
		copyonwrite_prepare(client);
	}

	setmysockopt(client->net);

	mainloop(client);
	do_run(client->server->postrun, client->exportname);

	if (-1 != client->transactionlogfd)
	{
		close(client->transactionlogfd);
		client->transactionlogfd = -1;
	}
}
