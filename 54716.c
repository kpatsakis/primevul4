static void hid_cancel_delayed_stuff(struct usbhid_device *usbhid)
{
	del_timer_sync(&usbhid->io_retry);
	cancel_work_sync(&usbhid->reset_work);
}
