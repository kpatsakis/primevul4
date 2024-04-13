void usb_set_device_state(struct usb_device *udev,
		enum usb_device_state new_state)
{
	unsigned long flags;
	int wakeup = -1;

	spin_lock_irqsave(&device_state_lock, flags);
	if (udev->state == USB_STATE_NOTATTACHED)
		;	/* do nothing */
	else if (new_state != USB_STATE_NOTATTACHED) {

		/* root hub wakeup capabilities are managed out-of-band
		 * and may involve silicon errata ... ignore them here.
		 */
		if (udev->parent) {
			if (udev->state == USB_STATE_SUSPENDED
					|| new_state == USB_STATE_SUSPENDED)
				;	/* No change to wakeup settings */
			else if (new_state == USB_STATE_CONFIGURED)
				wakeup = (udev->quirks &
					USB_QUIRK_IGNORE_REMOTE_WAKEUP) ? 0 :
					udev->actconfig->desc.bmAttributes &
					USB_CONFIG_ATT_WAKEUP;
			else
				wakeup = 0;
		}
		if (udev->state == USB_STATE_SUSPENDED &&
			new_state != USB_STATE_SUSPENDED)
			udev->active_duration -= jiffies;
		else if (new_state == USB_STATE_SUSPENDED &&
				udev->state != USB_STATE_SUSPENDED)
			udev->active_duration += jiffies;
		udev->state = new_state;
	} else
		recursively_mark_NOTATTACHED(udev);
	spin_unlock_irqrestore(&device_state_lock, flags);
	if (wakeup >= 0)
		device_set_wakeup_capable(&udev->dev, wakeup);
}
