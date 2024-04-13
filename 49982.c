bsg_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct bsg_device *bd = file->private_data;
	int ret;
	ssize_t bytes_read;

	dprintk("%s: read %Zd bytes\n", bd->name, count);

	bsg_set_block(bd, file);

	bytes_read = 0;
	ret = __bsg_read(buf, count, bd, NULL, &bytes_read);
	*ppos = bytes_read;

	if (!bytes_read || err_block_err(ret))
		bytes_read = ret;

	return bytes_read;
}
