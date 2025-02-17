static void dns_resolver_describe(const struct key *key, struct seq_file *m)
{
	int err = key->type_data.x[0];

	seq_puts(m, key->description);
	if (key_is_instantiated(key)) {
		if (err)
			seq_printf(m, ": %d", err);
		else
			seq_printf(m, ": %u", key->datalen);
	}
}
