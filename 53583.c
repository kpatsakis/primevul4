static void hub_tt_work(struct work_struct *work)
{
	struct usb_hub		*hub =
		container_of(work, struct usb_hub, tt.clear_work);
	unsigned long		flags;

	spin_lock_irqsave(&hub->tt.lock, flags);
	while (!list_empty(&hub->tt.clear_list)) {
		struct list_head	*next;
		struct usb_tt_clear	*clear;
		struct usb_device	*hdev = hub->hdev;
		const struct hc_driver	*drv;
		int			status;

		next = hub->tt.clear_list.next;
		clear = list_entry(next, struct usb_tt_clear, clear_list);
		list_del(&clear->clear_list);

		/* drop lock so HCD can concurrently report other TT errors */
		spin_unlock_irqrestore(&hub->tt.lock, flags);
		status = hub_clear_tt_buffer(hdev, clear->devinfo, clear->tt);
		if (status && status != -ENODEV)
			dev_err(&hdev->dev,
				"clear tt %d (%04x) error %d\n",
				clear->tt, clear->devinfo, status);

		/* Tell the HCD, even if the operation failed */
		drv = clear->hcd->driver;
		if (drv->clear_tt_buffer_complete)
			(drv->clear_tt_buffer_complete)(clear->hcd, clear->ep);

		kfree(clear);
		spin_lock_irqsave(&hub->tt.lock, flags);
	}
	spin_unlock_irqrestore(&hub->tt.lock, flags);
}
