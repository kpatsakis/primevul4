static int __init usbtest_init(void)
{
#ifdef GENERIC
	if (vendor)
		pr_debug("params: vend=0x%04x prod=0x%04x\n", vendor, product);
#endif
	return usb_register(&usbtest_driver);
}
