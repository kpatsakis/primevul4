void vhost_dev_init(struct vhost_dev *dev,
		    struct vhost_virtqueue **vqs, int nvqs)
{
	struct vhost_virtqueue *vq;
	int i;

	dev->vqs = vqs;
	dev->nvqs = nvqs;
	mutex_init(&dev->mutex);
	dev->log_ctx = NULL;
	dev->log_file = NULL;
	dev->memory = NULL;
	dev->mm = NULL;
	spin_lock_init(&dev->work_lock);
	INIT_LIST_HEAD(&dev->work_list);
	dev->worker = NULL;

	for (i = 0; i < dev->nvqs; ++i) {
		vq = dev->vqs[i];
		vq->log = NULL;
		vq->indirect = NULL;
		vq->heads = NULL;
		vq->dev = dev;
		mutex_init(&vq->mutex);
		vhost_vq_reset(dev, vq);
		if (vq->handle_kick)
			vhost_poll_init(&vq->poll, vq->handle_kick,
					POLLIN, dev);
	}
}
