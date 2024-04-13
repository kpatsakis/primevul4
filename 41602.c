static int wdm_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct wdm_device *desc = wdm_find_device(intf);
	int rv = 0;

	dev_dbg(&desc->intf->dev, "wdm%d_suspend\n", intf->minor);

	/* if this is an autosuspend the caller does the locking */
	if (!PMSG_IS_AUTO(message)) {
		mutex_lock(&desc->rlock);
		mutex_lock(&desc->wlock);
	}
	spin_lock_irq(&desc->iuspin);

	if (PMSG_IS_AUTO(message) &&
			(test_bit(WDM_IN_USE, &desc->flags)
			|| test_bit(WDM_RESPONDING, &desc->flags))) {
		spin_unlock_irq(&desc->iuspin);
		rv = -EBUSY;
	} else {

		set_bit(WDM_SUSPENDING, &desc->flags);
		spin_unlock_irq(&desc->iuspin);
		/* callback submits work - order is essential */
		kill_urbs(desc);
		cancel_work_sync(&desc->rxwork);
	}
	if (!PMSG_IS_AUTO(message)) {
		mutex_unlock(&desc->wlock);
		mutex_unlock(&desc->rlock);
	}

	return rv;
}
