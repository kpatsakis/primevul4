static void m_stop(struct seq_file *m, void *v)
{
	up_read(&namespace_sem);
}
