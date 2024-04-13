static void vhost_poll_func(struct file *file, wait_queue_head_t *wqh,
			    poll_table *pt)
{
	struct vhost_poll *poll;

	poll = container_of(pt, struct vhost_poll, table);
	poll->wqh = wqh;
	add_wait_queue(wqh, &poll->wait);
}
