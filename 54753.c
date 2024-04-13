static void usbhid_submit_report(struct hid_device *hid, struct hid_report *report, unsigned char dir)
{
	struct usbhid_device *usbhid = hid->driver_data;
	unsigned long flags;

	spin_lock_irqsave(&usbhid->lock, flags);
	__usbhid_submit_report(hid, report, dir);
	spin_unlock_irqrestore(&usbhid->lock, flags);
}
