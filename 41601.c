static void wdm_rxwork(struct work_struct *work)
{
	struct wdm_device *desc = container_of(work, struct wdm_device, rxwork);
	unsigned long flags;
	int rv;

	spin_lock_irqsave(&desc->iuspin, flags);
	if (test_bit(WDM_DISCONNECTING, &desc->flags)) {
		spin_unlock_irqrestore(&desc->iuspin, flags);
	} else {
		spin_unlock_irqrestore(&desc->iuspin, flags);
		rv = usb_submit_urb(desc->response, GFP_KERNEL);
		if (rv < 0 && rv != -EPERM) {
			spin_lock_irqsave(&desc->iuspin, flags);
			if (!test_bit(WDM_DISCONNECTING, &desc->flags))
				schedule_work(&desc->rxwork);
			spin_unlock_irqrestore(&desc->iuspin, flags);
		}
	}
}
