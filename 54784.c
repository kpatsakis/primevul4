static struct usb_device *testdev_to_usbdev(struct usbtest_dev *test)
{
	return interface_to_usbdev(test->intf);
}
