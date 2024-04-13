void big_key_describe(const struct key *key, struct seq_file *m)
{
	unsigned long datalen = key->type_data.x[1];

	seq_puts(m, key->description);

	if (key_is_instantiated(key))
		seq_printf(m, ": %lu [%s]",
			   datalen,
			   datalen > BIG_KEY_FILE_THRESHOLD ? "file" : "buff");
}
