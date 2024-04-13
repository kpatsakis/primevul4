int oz_hcd_init(void)
{
	int err;

	if (usb_disabled())
		return -ENODEV;

	oz_urb_link_cache = KMEM_CACHE(oz_urb_link, 0);
	if (!oz_urb_link_cache)
		return -ENOMEM;

	tasklet_init(&g_urb_process_tasklet, oz_urb_process_tasklet, 0);
	tasklet_init(&g_urb_cancel_tasklet, oz_urb_cancel_tasklet, 0);
	err = platform_driver_register(&g_oz_plat_drv);
	oz_dbg(ON, "platform_driver_register() returned %d\n", err);
	if (err)
		goto error;
	g_plat_dev = platform_device_alloc(OZ_PLAT_DEV_NAME, -1);
	if (g_plat_dev == NULL) {
		err = -ENOMEM;
		goto error1;
	}
	oz_dbg(ON, "platform_device_alloc() succeeded\n");
	err = platform_device_add(g_plat_dev);
	if (err)
		goto error2;
	oz_dbg(ON, "platform_device_add() succeeded\n");
	return 0;
error2:
	platform_device_put(g_plat_dev);
error1:
	platform_driver_unregister(&g_oz_plat_drv);
error:
	tasklet_disable(&g_urb_process_tasklet);
	tasklet_disable(&g_urb_cancel_tasklet);
	oz_dbg(ON, "oz_hcd_init() failed %d\n", err);
	return err;
}
