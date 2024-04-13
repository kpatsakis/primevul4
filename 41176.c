static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct proc_mounts *p = proc_mounts(m);

	return seq_list_next(v, &p->ns->list, pos);
}
