static void client_dead(int ci)
{
	struct client *c = clients + ci;

	if (c->fd != -1) {
		log_debug("removing client %d", c->fd);
		close(c->fd);
	}

	c->fd = -1;
	c->workfn = NULL;

	if (c->msg) {
		free(c->msg);
		c->msg = NULL;
		c->offset = 0;
	}

	pollfds[ci].fd = -1;
}