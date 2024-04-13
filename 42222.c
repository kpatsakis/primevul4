void vhost_poll_start(struct vhost_poll *poll, struct file *file)
{
	unsigned long mask;

	mask = file->f_op->poll(file, &poll->table);
	if (mask)
		vhost_poll_wakeup(&poll->wait, 0, 0, (void *)mask);
}
