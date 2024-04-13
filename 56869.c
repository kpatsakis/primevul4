static struct dentry *mqueue_mount(struct file_system_type *fs_type,
			 int flags, const char *dev_name,
			 void *data)
{
	struct ipc_namespace *ns;
	if (flags & MS_KERNMOUNT) {
		ns = data;
		data = NULL;
	} else {
		ns = current->nsproxy->ipc_ns;
	}
	return mount_ns(fs_type, flags, data, ns, ns->user_ns, mqueue_fill_super);
}
