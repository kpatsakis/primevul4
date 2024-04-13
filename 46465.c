int vhost_poll_start(struct vhost_poll *poll, struct file *file)
{
	unsigned long mask;
	int ret = 0;

	if (poll->wqh)
		return 0;

	mask = file->f_op->poll(file, &poll->table);
	if (mask)
		vhost_poll_wakeup(&poll->wait, 0, 0, (void *)mask);
	if (mask & POLLERR) {
		if (poll->wqh)
			remove_wait_queue(poll->wqh, &poll->wait);
		ret = -EINVAL;
	}

	return ret;
}
