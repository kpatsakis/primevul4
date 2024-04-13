void vhost_poll_stop(struct vhost_poll *poll)
{
	remove_wait_queue(poll->wqh, &poll->wait);
}
