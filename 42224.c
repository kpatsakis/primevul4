static int vhost_poll_wakeup(wait_queue_t *wait, unsigned mode, int sync,
			     void *key)
{
	struct vhost_poll *poll = container_of(wait, struct vhost_poll, wait);

	if (!((unsigned long)key & poll->mask))
		return 0;

	vhost_poll_queue(poll);
	return 0;
}
