static ssize_t debugfs_read(struct file *filp, char __user *ubuf,
			    size_t count, loff_t *offp)
{
	struct port *port;
	char *buf;
	ssize_t ret, out_offset, out_count;

	out_count = 1024;
	buf = kmalloc(out_count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	port = filp->private_data;
	out_offset = 0;
	out_offset += snprintf(buf + out_offset, out_count,
			       "name: %s\n", port->name ? port->name : "");
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "guest_connected: %d\n", port->guest_connected);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "host_connected: %d\n", port->host_connected);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "outvq_full: %d\n", port->outvq_full);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "bytes_sent: %lu\n", port->stats.bytes_sent);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "bytes_received: %lu\n",
			       port->stats.bytes_received);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "bytes_discarded: %lu\n",
			       port->stats.bytes_discarded);
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "is_console: %s\n",
			       is_console_port(port) ? "yes" : "no");
	out_offset += snprintf(buf + out_offset, out_count - out_offset,
			       "console_vtermno: %u\n", port->cons.vtermno);

	ret = simple_read_from_buffer(ubuf, count, offp, buf, out_offset);
	kfree(buf);
	return ret;
}
