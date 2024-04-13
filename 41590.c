static void wdm_disconnect(struct usb_interface *intf)
{
	struct wdm_device *desc;
	unsigned long flags;

	usb_deregister_dev(intf, &wdm_class);
	desc = wdm_find_device(intf);
	mutex_lock(&wdm_mutex);

	/* the spinlock makes sure no new urbs are generated in the callbacks */
	spin_lock_irqsave(&desc->iuspin, flags);
	set_bit(WDM_DISCONNECTING, &desc->flags);
	set_bit(WDM_READ, &desc->flags);
	/* to terminate pending flushes */
	clear_bit(WDM_IN_USE, &desc->flags);
	spin_unlock_irqrestore(&desc->iuspin, flags);
	wake_up_all(&desc->wait);
	mutex_lock(&desc->rlock);
	mutex_lock(&desc->wlock);
	kill_urbs(desc);
	cancel_work_sync(&desc->rxwork);
	mutex_unlock(&desc->wlock);
	mutex_unlock(&desc->rlock);

	/* the desc->intf pointer used as list key is now invalid */
	spin_lock(&wdm_device_list_lock);
	list_del(&desc->device_list);
	spin_unlock(&wdm_device_list_lock);

	if (!desc->count)
		cleanup(desc);
	else
		dev_dbg(&intf->dev, "%s: %d open files - postponing cleanup\n", __func__, desc->count);
	mutex_unlock(&wdm_mutex);
}
