static void request_key_auth_describe(const struct key *key,
				      struct seq_file *m)
{
	struct request_key_auth *rka = key->payload.data;

	seq_puts(m, "key:");
	seq_puts(m, key->description);
	if (key_is_instantiated(key))
		seq_printf(m, " pid:%d ci:%zu", rka->pid, rka->callout_len);
}
