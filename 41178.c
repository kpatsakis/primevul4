static void *m_start(struct seq_file *m, loff_t *pos)
{
	struct proc_mounts *p = proc_mounts(m);

	down_read(&namespace_sem);
	return seq_list_start(&p->ns->list, *pos);
}
