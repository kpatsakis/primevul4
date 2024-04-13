static ssize_t poll_timeout_store(struct bus_type *bus, const char *buf,
				  size_t count)
{
	unsigned long long time;
	ktime_t hr_time;

	/* 120 seconds = maximum poll interval */
	if (sscanf(buf, "%llu\n", &time) != 1 || time < 1 ||
	    time > 120000000000ULL)
		return -EINVAL;
	poll_timeout = time;
	hr_time = ktime_set(0, poll_timeout);

	if (!hrtimer_is_queued(&ap_poll_timer) ||
	    !hrtimer_forward(&ap_poll_timer, hrtimer_get_expires(&ap_poll_timer), hr_time)) {
		hrtimer_set_expires(&ap_poll_timer, hr_time);
		hrtimer_start_expires(&ap_poll_timer, HRTIMER_MODE_ABS);
	}
	return count;
}
