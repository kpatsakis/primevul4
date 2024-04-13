static void __exit hid_exit(void)
{
	usb_deregister(&hid_driver);
	usbhid_quirks_exit();
}
