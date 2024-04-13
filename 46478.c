void vhost_work_queue(struct vhost_dev *dev, struct vhost_work *work)
{
	unsigned long flags;

	spin_lock_irqsave(&dev->work_lock, flags);
	if (list_empty(&work->node)) {
		list_add_tail(&work->node, &dev->work_list);
		work->queue_seq++;
		spin_unlock_irqrestore(&dev->work_lock, flags);
		wake_up_process(dev->worker);
	} else {
		spin_unlock_irqrestore(&dev->work_lock, flags);
	}
}
