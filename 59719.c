static void asymmetric_key_describe(const struct key *key, struct seq_file *m)
{
	const struct asymmetric_key_subtype *subtype = asymmetric_key_subtype(key);
	const char *kid = asymmetric_key_id(key);
	size_t n;

	seq_puts(m, key->description);

	if (subtype) {
		seq_puts(m, ": ");
		subtype->describe(key, m);

		if (kid) {
			seq_putc(m, ' ');
			n = strlen(kid);
			if (n <= 8)
				seq_puts(m, kid);
			else
				seq_puts(m, kid + n - 8);
		}

		seq_puts(m, " [");
		/* put something here to indicate the key's capabilities */
		seq_putc(m, ']');
	}
}
