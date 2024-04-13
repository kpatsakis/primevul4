static int __init hid_init(void)
{
	int ret;

	if (hid_debug)
		pr_warn("hid_debug is now used solely for parser and driver debugging.\n"
			"debugfs is now used for inspecting the device (report descriptor, reports)\n");

	ret = bus_register(&hid_bus_type);
	if (ret) {
		pr_err("can't register hid bus\n");
		goto err;
	}

	ret = hidraw_init();
	if (ret)
		goto err_bus;

	hid_debug_init();

	return 0;
err_bus:
	bus_unregister(&hid_bus_type);
err:
	return ret;
}
