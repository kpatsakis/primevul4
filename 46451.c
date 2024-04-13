void vhost_dev_stop(struct vhost_dev *dev)
{
	int i;

	for (i = 0; i < dev->nvqs; ++i) {
		if (dev->vqs[i]->kick && dev->vqs[i]->handle_kick) {
			vhost_poll_stop(&dev->vqs[i]->poll);
			vhost_poll_flush(&dev->vqs[i]->poll);
		}
	}
}
