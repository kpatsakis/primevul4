static void client_alloc(void)
{
	int i;

	if (!(clients = realloc(
		clients, (client_size + CLIENT_NALLOC) * sizeof(*clients))
	) || !(pollfds = realloc(
		pollfds, (client_size + CLIENT_NALLOC) * sizeof(*pollfds))
	)) {
		log_error("can't alloc for client array");
		exit(1);
	}

	for (i = client_size; i < client_size + CLIENT_NALLOC; i++) {
		clients[i].workfn = NULL;
		clients[i].deadfn = NULL;
		clients[i].fd = -1;
		pollfds[i].fd = -1;
		pollfds[i].revents = 0;
	}
	client_size += CLIENT_NALLOC;
}