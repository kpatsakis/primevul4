static int sysvipc_proc_open(struct inode *inode, struct file *file)
{
	struct ipc_proc_iter *iter;

	iter = __seq_open_private(file, &sysvipc_proc_seqops, sizeof(*iter));
	if (!iter)
		return -ENOMEM;

	iter->iface = PDE_DATA(inode);
	iter->ns    = get_ipc_ns(current->nsproxy->ipc_ns);

	return 0;
}
