static char *ffs_prepare_buffer(const char __user *buf, size_t len)
{
	char *data;

	if (unlikely(!len))
		return NULL;

	data = kmalloc(len, GFP_KERNEL);
	if (unlikely(!data))
		return ERR_PTR(-ENOMEM);

	if (unlikely(copy_from_user(data, buf, len))) {
		kfree(data);
		return ERR_PTR(-EFAULT);
	}

	pr_vdebug("Buffer from user space:\n");
	ffs_dump_mem("", data, len);

	return data;
}
