static struct trusted_key_payload *trusted_payload_alloc(struct key *key)
{
	struct trusted_key_payload *p = NULL;
	int ret;

	ret = key_payload_reserve(key, sizeof *p);
	if (ret < 0)
		return p;
	p = kzalloc(sizeof *p, GFP_KERNEL);
	if (p)
		p->migratable = 1; /* migratable by default */
	return p;
}
