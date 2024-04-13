void ap_module_exit(void)
{
	int i;
	struct device *dev;

	ap_reset_domain();
	ap_poll_thread_stop();
	del_timer_sync(&ap_config_timer);
	hrtimer_cancel(&ap_poll_timer);
	destroy_workqueue(ap_work_queue);
	tasklet_kill(&ap_tasklet);
	root_device_unregister(ap_root_device);
	while ((dev = bus_find_device(&ap_bus_type, NULL, NULL,
		    __ap_match_all)))
	{
		device_unregister(dev);
		put_device(dev);
	}
	for (i = 0; ap_bus_attrs[i]; i++)
		bus_remove_file(&ap_bus_type, ap_bus_attrs[i]);
	bus_unregister(&ap_bus_type);
	unregister_reset_call(&ap_reset_call);
	if (ap_using_interrupts())
		unregister_adapter_interrupt(&ap_airq);
}
