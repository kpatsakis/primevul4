static void *ax25_info_next(struct seq_file *seq, void *v, loff_t *pos)
{
	return seq_hlist_next(v, &ax25_list, pos);
}
