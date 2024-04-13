void ap_bus_force_rescan(void)
{
	/* reconfigure the AP bus rescan timer. */
	mod_timer(&ap_config_timer, jiffies + ap_config_time * HZ);
	/* processing a asynchronous bus rescan */
	queue_work(ap_work_queue, &ap_config_work);
	flush_work(&ap_config_work);
}
