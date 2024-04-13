static inline int addr_match(struct snd_seq_addr *r, struct snd_seq_addr *s)
{
	return (r->client == s->client) && (r->port == s->port);
}
