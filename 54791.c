static void __exit usbtest_exit(void)
{
	usb_deregister(&usbtest_driver);
}
