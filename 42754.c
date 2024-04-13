static void *irias_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN)
		? (void *) hashbin_get_first(irias_objects)
		: (void *) hashbin_get_next(irias_objects);
}
