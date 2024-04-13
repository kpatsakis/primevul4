void vhost_poll_stop(struct vhost_poll *poll)
{
	if (poll->wqh) {
		remove_wait_queue(poll->wqh, &poll->wait);
		poll->wqh = NULL;
	}
}
