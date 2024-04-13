void vhost_dev_cleanup(struct vhost_dev *dev, bool locked)
{
	int i;

	for (i = 0; i < dev->nvqs; ++i) {
		if (dev->vqs[i]->error_ctx)
			eventfd_ctx_put(dev->vqs[i]->error_ctx);
		if (dev->vqs[i]->error)
			fput(dev->vqs[i]->error);
		if (dev->vqs[i]->kick)
			fput(dev->vqs[i]->kick);
		if (dev->vqs[i]->call_ctx)
			eventfd_ctx_put(dev->vqs[i]->call_ctx);
		if (dev->vqs[i]->call)
			fput(dev->vqs[i]->call);
		vhost_vq_reset(dev, dev->vqs[i]);
	}
	vhost_dev_free_iovecs(dev);
	if (dev->log_ctx)
		eventfd_ctx_put(dev->log_ctx);
	dev->log_ctx = NULL;
	if (dev->log_file)
		fput(dev->log_file);
	dev->log_file = NULL;
	/* No one will access memory at this point */
	kvfree(dev->memory);
	dev->memory = NULL;
	WARN_ON(!list_empty(&dev->work_list));
	if (dev->worker) {
		kthread_stop(dev->worker);
		dev->worker = NULL;
	}
	if (dev->mm)
		mmput(dev->mm);
	dev->mm = NULL;
}
