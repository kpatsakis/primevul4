static void *bt_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct bt_seq_state *s = seq->private;
	struct bt_sock_list *l = s->l;

	return seq_hlist_next(v, &l->head, pos);
}
