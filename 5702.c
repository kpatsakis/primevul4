int client_add(int fd, const struct booth_transport *tpt,
		void (*workfn)(int ci),
		void (*deadfn)(int ci))
{
	int i;
	struct client *c;


	if (client_size - 1 <= client_maxi ) {
		client_alloc();
	}

	for (i = 0; i < client_size; i++) {
		c = clients + i;
		if (c->fd != -1)
			continue;

		c->workfn = workfn;
		if (deadfn)
			c->deadfn = deadfn;
		else
			c->deadfn = client_dead;

		c->transport = tpt;
		c->fd = fd;
		c->msg = NULL;
		c->offset = 0;

		pollfds[i].fd = fd;
		pollfds[i].events = POLLIN;
		if (i > client_maxi)
			client_maxi = i;

		return i;
	}

	assert(!"no client");
}