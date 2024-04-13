int usb_hub_init(void)
{
	if (usb_register(&hub_driver) < 0) {
		printk(KERN_ERR "%s: can't register hub driver\n",
			usbcore_name);
		return -1;
	}

	/*
	 * The workqueue needs to be freezable to avoid interfering with
	 * USB-PERSIST port handover. Otherwise it might see that a full-speed
	 * device was gone before the EHCI controller had handed its port
	 * over to the companion full-speed controller.
	 */
	hub_wq = alloc_workqueue("usb_hub_wq", WQ_FREEZABLE, 0);
	if (hub_wq)
		return 0;

	/* Fall through if kernel_thread failed */
	usb_deregister(&hub_driver);
	pr_err("%s: can't allocate workqueue for usb hub\n", usbcore_name);

	return -1;
}
