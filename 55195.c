static int match_subs_info(struct snd_seq_port_subscribe *r,
			   struct snd_seq_port_subscribe *s)
{
	if (addr_match(&r->sender, &s->sender) &&
	    addr_match(&r->dest, &s->dest)) {
		if (r->flags && r->flags == s->flags)
			return r->queue == s->queue;
		else if (! r->flags)
			return 1;
	}
	return 0;
}
