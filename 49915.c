static int ap_device_remove(struct device *dev)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	struct ap_driver *ap_drv = ap_dev->drv;

	ap_flush_queue(ap_dev);
	del_timer_sync(&ap_dev->timeout);
	spin_lock_bh(&ap_device_list_lock);
	list_del_init(&ap_dev->list);
	spin_unlock_bh(&ap_device_list_lock);
	if (ap_drv->remove)
		ap_drv->remove(ap_dev);
	spin_lock_bh(&ap_dev->lock);
	atomic_sub(ap_dev->queue_count, &ap_poll_requests);
	spin_unlock_bh(&ap_dev->lock);
	return 0;
}
