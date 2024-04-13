static long vhost_dev_alloc_iovecs(struct vhost_dev *dev)
{
	struct vhost_virtqueue *vq;
	int i;

	for (i = 0; i < dev->nvqs; ++i) {
		vq = dev->vqs[i];
		vq->indirect = kmalloc(sizeof *vq->indirect * UIO_MAXIOV,
				       GFP_KERNEL);
		vq->log = kmalloc(sizeof *vq->log * UIO_MAXIOV, GFP_KERNEL);
		vq->heads = kmalloc(sizeof *vq->heads * UIO_MAXIOV, GFP_KERNEL);
		if (!vq->indirect || !vq->log || !vq->heads)
			goto err_nomem;
	}
	return 0;

err_nomem:
	for (; i >= 0; --i)
		vhost_vq_free_iovecs(dev->vqs[i]);
	return -ENOMEM;
}
