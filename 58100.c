static ssize_t port_fops_write(struct file *filp, const char __user *ubuf,
			       size_t count, loff_t *offp)
{
	struct port *port;
	struct port_buffer *buf;
	ssize_t ret;
	bool nonblock;
	struct scatterlist sg[1];

	/* Userspace could be out to fool us */
	if (!count)
		return 0;

	port = filp->private_data;

	nonblock = filp->f_flags & O_NONBLOCK;

	ret = wait_port_writable(port, nonblock);
	if (ret < 0)
		return ret;

	count = min((size_t)(32 * 1024), count);

	buf = alloc_buf(port->out_vq, count, 0);
	if (!buf)
		return -ENOMEM;

	ret = copy_from_user(buf->buf, ubuf, count);
	if (ret) {
		ret = -EFAULT;
		goto free_buf;
	}

	/*
	 * We now ask send_buf() to not spin for generic ports -- we
	 * can re-use the same code path that non-blocking file
	 * descriptors take for blocking file descriptors since the
	 * wait is already done and we're certain the write will go
	 * through to the host.
	 */
	nonblock = true;
	sg_init_one(sg, buf->buf, count);
	ret = __send_to_port(port, sg, 1, count, buf, nonblock);

	if (nonblock && ret > 0)
		goto out;

free_buf:
	free_buf(buf, true);
out:
	return ret;
}
