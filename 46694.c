static unsigned int mdstat_poll(struct file *filp, poll_table *wait)
{
	struct seq_file *seq = filp->private_data;
	int mask;

	if (md_unloading)
		return POLLIN|POLLRDNORM|POLLERR|POLLPRI;
	poll_wait(filp, &md_event_waiters, wait);

	/* always allow read */
	mask = POLLIN | POLLRDNORM;

	if (seq->poll_event != atomic_read(&md_event_count))
		mask |= POLLERR | POLLPRI;
	return mask;
}
