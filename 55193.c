static struct snd_seq_client_port *get_client_port(struct snd_seq_addr *addr,
						   struct snd_seq_client **cp)
{
	struct snd_seq_client_port *p;
	*cp = snd_seq_client_use_ptr(addr->client);
	if (*cp) {
		p = snd_seq_port_use_ptr(*cp, addr->port);
		if (! p) {
			snd_seq_client_unlock(*cp);
			*cp = NULL;
		}
		return p;
	}
	return NULL;
}
