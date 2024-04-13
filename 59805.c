static void rxrpc_describe(const struct key *key, struct seq_file *m)
{
	seq_puts(m, key->description);
}
