static ssize_t ap_config_time_store(struct bus_type *bus,
				    const char *buf, size_t count)
{
	int time;

	if (sscanf(buf, "%d\n", &time) != 1 || time < 5 || time > 120)
		return -EINVAL;
	ap_config_time = time;
	if (!timer_pending(&ap_config_timer) ||
	    !mod_timer(&ap_config_timer, jiffies + ap_config_time * HZ)) {
		ap_config_timer.expires = jiffies + ap_config_time * HZ;
		add_timer(&ap_config_timer);
	}
	return count;
}
