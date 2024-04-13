static int __init hid_init(void)
{
	int retval = -ENOMEM;

	retval = usbhid_quirks_init(quirks_param);
	if (retval)
		goto usbhid_quirks_init_fail;
	retval = usb_register(&hid_driver);
	if (retval)
		goto usb_register_fail;
	pr_info(KBUILD_MODNAME ": " DRIVER_DESC "\n");

	return 0;
usb_register_fail:
	usbhid_quirks_exit();
usbhid_quirks_init_fail:
	return retval;
}
