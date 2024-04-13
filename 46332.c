static int sysvipc_proc_show(struct seq_file *s, void *it)
{
	struct ipc_proc_iter *iter = s->private;
	struct ipc_proc_iface *iface = iter->iface;

	if (it == SEQ_START_TOKEN) {
		seq_puts(s, iface->header);
		return 0;
	}

	return iface->show(s, it);
}
