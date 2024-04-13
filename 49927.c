int __init ap_module_init(void)
{
	int rc, i;

	if (ap_domain_index < -1 || ap_domain_index >= AP_DOMAINS) {
		pr_warning("%d is not a valid cryptographic domain\n",
			   ap_domain_index);
		return -EINVAL;
	}
	/* In resume callback we need to know if the user had set the domain.
	 * If so, we can not just reset it.
	 */
	if (ap_domain_index >= 0)
		user_set_domain = 1;

	if (ap_instructions_available() != 0) {
		pr_warning("The hardware system does not support "
			   "AP instructions\n");
		return -ENODEV;
	}
	if (ap_interrupts_available()) {
		rc = register_adapter_interrupt(&ap_airq);
		ap_airq_flag = (rc == 0);
	}

	register_reset_call(&ap_reset_call);

	/* Create /sys/bus/ap. */
	rc = bus_register(&ap_bus_type);
	if (rc)
		goto out;
	for (i = 0; ap_bus_attrs[i]; i++) {
		rc = bus_create_file(&ap_bus_type, ap_bus_attrs[i]);
		if (rc)
			goto out_bus;
	}

	/* Create /sys/devices/ap. */
	ap_root_device = root_device_register("ap");
	rc = PTR_RET(ap_root_device);
	if (rc)
		goto out_bus;

	ap_work_queue = create_singlethread_workqueue("kapwork");
	if (!ap_work_queue) {
		rc = -ENOMEM;
		goto out_root;
	}

	ap_query_configuration();
	if (ap_select_domain() == 0)
		ap_scan_bus(NULL);

	/* Setup the AP bus rescan timer. */
	init_timer(&ap_config_timer);
	ap_config_timer.function = ap_config_timeout;
	ap_config_timer.data = 0;
	ap_config_timer.expires = jiffies + ap_config_time * HZ;
	add_timer(&ap_config_timer);

	/* Setup the high resultion poll timer.
	 * If we are running under z/VM adjust polling to z/VM polling rate.
	 */
	if (MACHINE_IS_VM)
		poll_timeout = 1500000;
	spin_lock_init(&ap_poll_timer_lock);
	hrtimer_init(&ap_poll_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
	ap_poll_timer.function = ap_poll_timeout;

	/* Start the low priority AP bus poll thread. */
	if (ap_thread_flag) {
		rc = ap_poll_thread_start();
		if (rc)
			goto out_work;
	}

	return 0;

out_work:
	del_timer_sync(&ap_config_timer);
	hrtimer_cancel(&ap_poll_timer);
	destroy_workqueue(ap_work_queue);
out_root:
	root_device_unregister(ap_root_device);
out_bus:
	while (i--)
		bus_remove_file(&ap_bus_type, ap_bus_attrs[i]);
	bus_unregister(&ap_bus_type);
out:
	unregister_reset_call(&ap_reset_call);
	if (ap_using_interrupts())
		unregister_adapter_interrupt(&ap_airq);
	return rc;
}
