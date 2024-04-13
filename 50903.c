static void __exit hid_exit(void)
{
	hid_debug_exit();
	hidraw_exit();
	bus_unregister(&hid_bus_type);
}
