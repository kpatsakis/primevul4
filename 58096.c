static int port_fops_fasync(int fd, struct file *filp, int mode)
{
	struct port *port;

	port = filp->private_data;
	return fasync_helper(fd, filp, mode, &port->async_queue);
}
