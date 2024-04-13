static int show_map(struct seq_file *m, void *v, int is_pid)
{
	show_map_vma(m, v, is_pid);
	m_cache_vma(m, v);
	return 0;
}
